#ifndef _LIB_VIDEO_ENC_H_
#define _LIB_VIDEO_ENC_H_

typedef enum VENC_CODEC_TYPE
{
	VENC_CODEC_H264,
	VENC_CODEC_JPEG,
	VENC_CODEC_VP8,
} VENC_CODEC_TYPE;

struct device_attr {
    int fps;
    int width;
    int height;
    int unk4;
    int unk5;
    int unk6;
};

struct VencH264FixQP
{
	int bEnable;
	int nIQp;
	int nPQp;
};

struct channel_params {
    VENC_CODEC_TYPE codecType;
    int eInputFormat;
    int nInputWidth;
    int nInputHeight;
    int nDstWidth;
    int nDstHeight;
    int bpsBitrate;
    int fps;
    struct VencH264FixQP fixedQP;
};

struct unkstruct1 {
    void *unk1; // buffer
    int unk2;   // largo de buffer
};

struct VI_VideoFrame {
    int unk1;
    int unk2;
    int unk3;
    int unk4;
    int unk5;
    int unk6;
    int unk7;
    int unk8;
    int unk9;
    int unk10;
    int unk11;
    int unk12;
    int unk13;
    int unk14;
};

struct unkstruct3 {
    int unk1;
    int unk2;
    int nFlag;      // Used to mark whether it is a keyframe
    int nSize0;     // The length of the first part of the output stream, the stored data is in the address pData0
    int nSize1;     // The length of the second part of the output code stream, the stored data is in the address pData1, when nSize1 = 0, the code stream is only a part, and the second part does not exist
    void *pData0;   // The first part of the output stream
    void *pData1;   // The second part of the output stream
    int unk8;
};

//
// VI = Video Interface?
//
int VI_EnableDev();
void VI_DisableDev();
int VI_SetDevAttr(struct device_attr *attr);
int VI_GetFrame(struct VI_VideoFrame *param1);
int VI_ReleaseFrame(struct VI_VideoFrame *param1);

//
// VENC = Video Encoder?
//
int VENC_Init();
int VENC_CreateChn(struct channel_params *params);
int VENC_DestroyChn(int channel_number);
int VENC_SendFrame(int channel_number, struct VI_VideoFrame *param1);
int VENC_GetStream(int channel_number, struct unkstruct3 *param1);
int VENC_ReleaseStream(int channel_number, struct unkstruct3 *param1);
int VENC_DeInit();

//
// VDEC = Video Decoder?
//
int VDEC_GetHeader(int channel_number, struct unkstruct1 *param2);

#endif /* _LIB_VIDEO_ENC_H_ */
