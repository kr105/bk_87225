#ifndef _LIB_AUDIO_IN_H_
#define _LIB_AUDIO_IN_H_

struct AI_audio_parameters {
    int sampleRate;
    int sampleBits;
    int syncMode;       // 1=Synchronous mode (direct call to read and write functions, may block) --- 0=Asynchronous mode (use callback functions)
    int channels;
    int cacheBlocks;    // The maximum number of buffered data blocks for recording
    int cacheBlockTime; // Duration of each buffered data block in milliseconds
    int unk7;
    int unk8;
};

struct VI_AudioFrame {
    void *unk1;
    void *unk2;
    void *unk3;
    void *unk4;
    void *unk5;
    void *unk6;
    void *unk7;
    int length;
};

int AI_Enable();
int AI_EnableAec();
int AI_EnableChn(struct AI_audio_parameters *param1);
int AI_GetFrame(int channelid, struct VI_AudioFrame *audioFrame, int timeout);

int AI_Disable();
int AI_DisableChn(int channelid);
int AI_DisableAec();




#endif /* _LIB_AUDIO_IN_H_ */
