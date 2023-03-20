#include <fcntl.h>
#include <memory.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <MQTTClient.h>

#include "include/gpio.h"
#include "include/mqtt.h"
#include "include/libAudioIn.h"
#include "include/libVideoEnc.h"

#define WDIOC_KEEPALIVE 0x80045705
#define WDIOC_SETTIMEOUT 0xc0045706

int s_watchdog_fd = 0;
int s_video_fifo_fd = 0;
int s_audio_fifo_fd = 0;

void *thread_watchdog(void *vargp)
{
    int ret;

    while (1)
    {
        if (s_watchdog_fd < 0)
        {
            printf("Watchdog not initialized!\n");
            return 0;
        }

        ret = ioctl(s_watchdog_fd, WDIOC_KEEPALIVE, NULL);

        if (ret != 0)
        {
            printf("WDIOC_KEEPALIVE fail!\n");
            return 0;
        }

        printf("Watchdog keepalive!\n");
        sleep(5);
    }
}

void *thread_video(void *vargp)
{
    int ret;

    ret = VI_EnableDev();
    if (ret == 0)
    {
        printf("VI_EnableDev fail!\n");
        return 0;
    }

    struct device_attr myattr;

    myattr.fps = 25;
    myattr.width = 1280;
    myattr.height = 720;
    myattr.unk4 = 2;
    myattr.unk5 = 0;
    myattr.unk6 = 0;

    ret = VI_SetDevAttr(&myattr);
    if (ret != 1)
    {
        printf("VI_SetDevAttr fail!\n");
        return 0;
    }

    ret = VENC_Init();
    if (ret == 0)
    {
        printf("VENC_Init fail!\n");
        return 0;
    }

    struct channel_params params;
    memset(&params, 0x00, sizeof(struct channel_params));
    params.codecType = VENC_CODEC_H264;
    params.eInputFormat = 1; // 1=VENC_PIXEL_YUV420SP // 2=VENC_PIXEL_YVU420P // Any other=VENC_PIXEL_YUV420P
    params.nInputWidth = 1280;
    params.nInputHeight = 720;
    params.nDstWidth = 1280;
    params.nDstHeight = 720;
    params.bpsBitrate = 3 * 1024 * 1024;
    params.fps = 25;
    params.fixedQP.bEnable = 0;
    params.fixedQP.nIQp = 30;
    params.fixedQP.nPQp = 45;

    int channelid = 0;
    channelid = VENC_CreateChn(&params);

    if (channelid >= 0)
    {
        sleep(1);

        struct unkstruct1 param01;
        struct VI_VideoFrame param02;
        struct unkstruct3 param03;

        mkfifo("/tmp/ap_video.bin", 0666);
        s_video_fifo_fd = open("/tmp/ap_video.bin", O_RDWR);
        // FILE *fp = fopen("/tmp/data1.bin", "wb");

        // struct timespec ts;

        while (1)
        {
            memset(&param01, 0x00, sizeof(struct unkstruct1));
            memset(&param02, 0x00, sizeof(struct VI_VideoFrame));
            memset(&param03, 0x00, sizeof(struct unkstruct3));

            ret = VDEC_GetHeader(channelid, &param01);
            if (ret == 0)
            {
                printf("VDEC_GetHeader fail!\n");
                return 0;
            }

            ret = VI_GetFrame(&param02);
            if (ret == 0)
            {
                printf("VI_GetFrame fail!\n");
                return 0;
            }

            ret = VENC_SendFrame(channelid, &param02);
            if (ret != 0)
            {
                printf("VENC_SendFrame fail!\n");
                return 0;
            }

            ret = VENC_GetStream(channelid, &param03);
            if (ret == 0)
            {
                printf("VENC_GetStream fail!\n");
                return 0;
            }

            // Keyframe
            if (param03.nFlag != 0)
            {
                // printf("nFlag = %d\n", param03.nFlag);
                // fwrite(param01.unk1, 1, param01.unk2, fp);
                write(s_video_fifo_fd, param01.unk1, param01.unk2);
            }

            if (param03.nSize0 > 0)
            {
                // fwrite(param03.pData0, 1, param03.nSize0, fp);
                write(s_video_fifo_fd, param03.pData0, param03.nSize0);

                if (param03.nSize1 > 0)
                {
                    // fwrite(param03.pData1, 1, param03.nSize1, fp);
                    write(s_video_fifo_fd, param03.pData1, param03.nSize1);
                }
            }

            ret = VENC_ReleaseStream(channelid, &param03);
            if (ret == 0)
            {
                printf("VENC_ReleaseStream fail!\n");
                return 0;
            }

            ret = VI_ReleaseFrame(&param02);
            if (ret == 0)
            {
                printf("VI_ReleaseFrame fail!\n");
                return 0;
            }
        }

        ret = VENC_DestroyChn(channelid);
        if (ret == 0)
        {
            printf("VENC_DestroyChn fail!\n");
            return 0;
        }

        VI_DisableDev();

        ret = VENC_DeInit();
        if (ret == 0)
        {
            printf("VENC_DeInit fail!\n");
            return 0;
        }

        // fclose(fp);
        close(s_video_fifo_fd);
    }

    return 0;
}

void *thread_audio(void *vargp)
{
    int ret;

    ret = AI_Enable();
    if (ret != 0)
    {
        printf("AI_Enable fail!\n");
        return 0;
    }

    // AI_EnableAec();

    struct AI_audio_parameters param01;
    memset(&param01, 0x00, sizeof(struct AI_audio_parameters));
    param01.sampleRate = 8000;
    param01.sampleBits = 16;
    param01.channels = 1;
    param01.cacheBlocks = 4;
    param01.cacheBlockTime = 20;

    int channelid = 0;
    channelid = AI_EnableChn(&param01);

    if (channelid < 0)
    {
        printf("AI_EnableChn fail!\n");
        return 0;
    }

    sleep(1);

    mkfifo("/tmp/ap_audio.bin", 0666);
    s_audio_fifo_fd = open("/tmp/ap_audio.bin", O_RDWR);

    while (1)
    {
        struct VI_AudioFrame audioFrame;

        memset(&audioFrame, 0x00, sizeof(struct VI_AudioFrame));
        ret = AI_GetFrame(channelid, &audioFrame, 160);

        if (ret != 0)
        {
            printf("AI_GetFrame fail!\n");
            continue;
        }

        write(s_audio_fifo_fd, audioFrame.unk1, audioFrame.length);
    }

    AI_DisableAec();
    AI_DisableChn(channelid);
    AI_Disable();

    return 0;
}

void startup_animation()
{
    gpio_set_value(70, "1");
    usleep(500000);
    gpio_set_value(70, "0");
    usleep(500000);

    gpio_set_value(36, "1");
    usleep(500000);
    gpio_set_value(36, "0");
    usleep(500000);

    gpio_set_value(200, "1");
    usleep(500000);
    gpio_set_value(200, "0");
    usleep(500000);
}

void button_push_animation()
{
    gpio_set_value(72, "1");
    usleep(500000);
    gpio_set_value(72, "0");
    usleep(500000);

    gpio_set_value(72, "1");
    usleep(500000);
    gpio_set_value(72, "0");
    usleep(500000);

    gpio_set_value(72, "1");
    usleep(500000);
    gpio_set_value(72, "0");
    usleep(500000);

    gpio_set_value(72, "1");
    usleep(500000);
    gpio_set_value(72, "0");
    usleep(500000);

    gpio_set_value(72, "1");
    usleep(500000);
    gpio_set_value(72, "0");
    usleep(500000);
}

void *thread_call_button(void *vargp)
{
    int read_value, ret;

    while (1)
    {
        gpio_set_value(70, "1");

        read_value = gpio_poll_value(44);

        // GPIO is read as 0 when the button is pressed
        if (read_value == 0)
        {
            ret = mqtt_send("{\"state\": \"pushed\"}", MQTT_TOPIC);

            gpio_set_value(70, "0");

            // this will delay the next event
            button_push_animation();
        }
    }

    return 0;
}

void *thread_mqtt(void *vargp)
{
    setup_mqtt();

    return 0;
}

int main()
{
    int ret;
    int timeout = 10;

    pthread_t thread_id_watchdog;
    pthread_t thread_id_video;
    pthread_t thread_id_audio;
    pthread_t thread_id_call_button;
    pthread_t thread_id_mqtt;

    s_watchdog_fd = open("/dev/watchdog", O_RDWR);

    if (s_watchdog_fd < 0)
    {
        printf("Opening /dev/watchdog failed!\n");
        return 0;
    }

    ret = ioctl(s_watchdog_fd, WDIOC_SETTIMEOUT, &timeout);

    if (ret != 0)
    {
        printf("WDIOC_SETTIMEOUT fail!\n");
        return 0;
    }

    pthread_create(&thread_id_watchdog, NULL, thread_watchdog, NULL);

    setup_gpio();
    startup_animation();

    sleep(3);

    pthread_create(&thread_id_video, NULL, thread_video, NULL);
    pthread_create(&thread_id_audio, NULL, thread_audio, NULL);
    pthread_create(&thread_id_call_button, NULL, thread_call_button, NULL);
    pthread_create(&thread_id_mqtt, NULL, thread_mqtt, NULL);

    while (1)
    {
        sleep(1);
    }

    return 1;
}