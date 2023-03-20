#ifndef MQTT_H
#define MQTT_H

#define BUFFER_SIZE 1024

int setup_mqtt();
int mqtt_send(char *payload, char *topic);

#endif