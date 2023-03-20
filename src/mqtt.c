#include <pthread.h>
#include <signal.h>

#include "MQTTClient.h"

#include "mqtt.h"

struct opts_struct
{
    char *clientid;
    int nodelimiter;
    char *delimiter;
    enum QoS qos;
    char *username;
    char *password;
    char *host;
    int port;
    int showtopics;
} opts;

static volatile int toStop = 0;
static Network n;
static MQTTClient c;
static pthread_mutex_t yield_lock;

void cfinish(int sig)
{
    signal(SIGINT, NULL);
    toStop = 1;
}

void messageArrived(MessageData *md)
{
    MQTTMessage *message = md->message;

    if (opts.showtopics)
        printf("%.*s\t", md->topicName->lenstring.len, md->topicName->lenstring.data);
    if (opts.nodelimiter)
        printf("%.*s", (int)message->payloadlen, (char *)message->payload);
    else
        printf("%.*s%s", (int)message->payloadlen, (char *)message->payload, opts.delimiter);
}

int mqtt_send(char *payload)
{
    int rc = 0;
    MQTTMessage message;

    memset(&message, 0x00, sizeof(MQTTMessage));

    message.qos = QOS2;
    message.retained = 0;
    message.payload = payload;
    message.payloadlen = strlen(payload);

    pthread_mutex_lock(&yield_lock);

    rc = MQTTPublish(&c, MQTT_TOPIC, &message);

    // Wait for QoS messages to be exchanged
    // MQTTYield(&c, 1000);

    pthread_mutex_unlock(&yield_lock);

    return rc;
}

int mqtt_register_home_assistant()
{
    int rc = 0;
    MQTTMessage message;

    memset(&message, 0x00, sizeof(MQTTMessage));

    char *payload = "{\
        \"automation_type\":\"trigger\",\
        \"topic\":\"" MQTT_TOPIC "\",\
        \"payload\":\"pushed\",\
        \"type\":\"button_short_press\",\
        \"subtype\":\"button_1\",\
        \"device\":{\
            \"model\":\"83225EPC-WIFI\"\
        }\
    }";

    message.qos = QOS2;
    message.retained = 0;
    message.payload = payload;
    message.payloadlen = strlen(payload);

    pthread_mutex_lock(&yield_lock);

    rc = MQTTPublish(&c, MQTT_TOPIC, &message);

    // Wait for QoS messages to be exchanged
    // MQTTYield(&c, 1000);

    pthread_mutex_unlock(&yield_lock);

    return rc;
}

int setup_mqtt()
{
    int rc = 0;
    unsigned char buf[100];
    unsigned char readbuf[100];

    memset(&opts, 0x00, sizeof(struct opts_struct));

    opts.clientid = "bk87225-subscriber";
    opts.delimiter = "\n";
    opts.qos = QOS2;
    opts.username = MQTT_USERNAME;
    opts.password = MQTT_PASSWORD;
    opts.host = MQTT_HOST;
    opts.port = MQTT_PORT;

    // signal(SIGINT, cfinish);
    // signal(SIGTERM, cfinish);

    NetworkInit(&n);
    NetworkConnect(&n, opts.host, opts.port);

    MQTTClientInit(&c, &n, 1000, buf, 100, readbuf, 100);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = opts.clientid;
    data.username.cstring = opts.username;
    data.password.cstring = opts.password;

    data.keepAliveInterval = 10;
    data.cleansession = 1;
    printf("Connecting to %s %d\n", opts.host, opts.port);

    rc = MQTTConnect(&c, &data);
    printf("Connected %d\n", rc);

    printf("Subscribing to %s\n", MQTT_TOPIC);
    rc = MQTTSubscribe(&c, MQTT_TOPIC, opts.qos, messageArrived);
    printf("Subscribed %d\n", rc);

    pthread_mutex_init(&yield_lock, NULL);

    while (!toStop)
    {
        if (pthread_mutex_trylock(&yield_lock) == 0)
        {
            MQTTYield(&c, 10);
            pthread_mutex_unlock(&yield_lock);
        }
    }

    printf("Stopping\n");

    MQTTDisconnect(&c);
    NetworkDisconnect(&n);

    return 1;
}