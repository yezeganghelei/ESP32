/**
 ****************************************************************************************************
 * @file        avi.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       AVIvideo格式解析 Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __AVI_H
#define __AVI_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "exfuns.h"
#include "ff.h"

/* frame size, Set according to your own memory application situation.
 * If there is too much memory,Can set it a little larger. If there is little memory,Set it smaller.
 * Generally, set this value equal to our frame cache size
 */
#define AVI_MAX_FRAME_SIZE  60 * 1024     /* 最大frame size,Can't exceed60KB */

/* Error Type */
typedef enum
{
    AVI_OK = 0,             /* 0,success */
    AVI_RIFF_ERR,           /* 1,RIFF IDRead failed */
    AVI_AVI_ERR,            /* 2,AVI  IDRead failed */
    AVI_LIST_ERR,           /* 3,LIST IDRead failed */
    AVI_HDRL_ERR,           /* 4,HDRL IDRead failed */
    AVI_AVIH_ERR,           /* 5,AVIH IDRead failed */
    AVI_STRL_ERR,           /* 6,STRL IDRead failed */
    AVI_STRH_ERR,           /* 7,STRH IDRead failed */
    AVI_STRF_ERR,           /* 8,STRF IDRead failed */
    AVI_MOVI_ERR,           /* 9,MOVI IDRead failed */
    AVI_FORMAT_ERR,         /* 10, format error */
    AVI_STREAM_ERR,         /* 11,Stream error */
} AVISTATUS;

#define AVI_RIFF_ID         0X46464952
#define AVI_AVI_ID          0X20495641
#define AVI_LIST_ID         0X5453494C
#define AVI_HDRL_ID         0X6C726468      /* 信息Block logo */
#define AVI_MOVI_ID         0X69766F6D      /* Data blocksLogo */
#define AVI_STRL_ID         0X6C727473      /* strl flag */

#define AVI_AVIH_ID         0X68697661      /* avih子piece∈AVI_HDRL_ID */
#define AVI_STRH_ID         0X68727473      /* strh(Flow head)子piece∈AVI_STRL_ID */
#define AVI_STRF_ID         0X66727473      /* strf(Stream format)子piece∈AVI_STRL_ID */
#define AVI_STRD_ID         0X64727473      /* strd子piece∈AVI_STRL_ID (Optional) */

#define AVI_VIDS_STREAM     0X73646976      /* video流 */
#define AVI_AUDS_STREAM     0X73647561      /* Audio流 */

#define AVI_VIDS_FLAG       0X6463          /* video流Logo */
#define AVI_AUDS_FLAG       0X7762          /* Audio Streaming Logo */

#define AVI_FORMAT_MJPG     0X47504A4D

/* AVI information structure */
/* Add some important data,Store here,Convenient decoding */
typedef struct
{
    uint32_t SecPerFrame;       /* Video frame interval time (unit is us) */
    uint32_t TotalFrame;        /* 文件总frame数 */
    uint32_t Width;             /* image width */
    uint32_t Height;            /* image height */
    uint32_t SampleRate;        /* Audio sampling rate */
    uint16_t Channels;          /* Number of channels,Generally2,express立体声 */
    uint16_t AudioBufSize;      /* Audio缓冲区大小 */
    uint16_t AudioType;         /* Audio类型:0X0001=PCM;0X0050=MP2;0X0055=MP3;0X2000=AC3; */
    uint16_t StreamID;          /* Stream type ID,StreamID=='dc'==0X6463 /StreamID=='wb'==0X7762 */
    uint32_t StreamSize;        /* Stream size,Must be an even number,If read an odd number,Add1.complement to even number */
    char *VideoFLAG;            /* Video frame marking,VideoFLAG="00dc"/"01dc" */
    char *AudioFLAG;            /* Audio frame tag,AudioFLAG="00wb"/"01wb" */
} AVI_INFO;

extern AVI_INFO g_avix;           /* aviFile related information */

/* AVI piece信息 */
typedef struct
{
    uint32_t RiffID;            /* RiffID=='RIFF'==0X61766968 */
    uint32_t FileSize;          /* AVIFile size(does not include the initial8byte,alsoRIFFIDandFileSizeNot counted) */
    uint32_t AviID;             /* AviID=='AVI '==0X41564920 */
} AVI_HEADER;

/* AVI piece信息 */
typedef struct
{
    uint32_t FrameID;           /* Frame ID,FrameID=='RIFF'==0X61766968 */
    uint32_t FrameSize;         /* frame size */
} FRAME_HEADER;

/* LIST piece信息 */
typedef struct
{
    uint32_t ListID;            /* ListID=='LIST'==0X4c495354 */
    uint32_t BlockSize;         /* piece大小(does not include the initial8byte,alsoListIDandBlockSizeNot counted) */
    uint32_t ListType;          /* LISTsub-block type:hdrl(Information block)/movi(Data blocks)/idxl(索引piece,No need,Is optional) */
} LIST_HEADER;

/* avih 子piece信息 */
typedef struct
{
    uint32_t BlockID;           /* Block logo:avih==0X61766968 */
    uint32_t BlockSize;         /* Block size (not including the initial 8 bytes, that is, BlockID and BlockSize are not counted) */
    uint32_t SecPerFrame;       /* Video frame interval time (unit is us) */
    uint32_t MaxByteSec;        /* Maximum data transmission rate,byte/Second */
    uint32_t PaddingGranularity;/* Granularity of data filling */
    uint32_t Flags;             /* Global tags of AVI files, such as whether they contain index blocks, etc. */
    uint32_t TotalFrame;        /* 文件总frame数 */
    uint32_t InitFrames;        /* Specify the initial number of frames for the interactive format（The non-interactive format should be specified as0） */
    uint32_t Streams;           /* Include的Data flow types个数,Usually2 */
    uint32_t RefBufSize;        /* It is recommended to read the cache size of this file（应能容纳最大的piece）The default may be1Mbyte!!! */
    uint32_t Width;             /* image width */
    uint32_t Height;            /* image height */
    uint32_t Reserved[4];       /* reserve */
} AVIH_HEADER;

/* strh Streaming header block information(strh∈strl) */
typedef struct
{
    uint32_t BlockID;       /* Block logo:strh==0X73747268 */
    uint32_t BlockSize;     /* Block size (not including the initial 8 bytes, that is, BlockID and BlockSize are not counted) */
    uint32_t StreamType;    /* Data flow types，vids(0X73646976):video;auds(0X73647561):Audio */
    uint32_t Handler;       /* Specify the stream handler，for音video来说就是Decoder,for exampleMJPG/H264Something like that */
    uint32_t Flags;         /* mark：Whether to allow this stream output？Does the color palette change？ */
    uint16_t Priority;      /* stream priority（When there are multiple streams of the same type, the highest priority is the default stream） */
    uint16_t Language;      /* Audio的语言代号 */
    uint32_t InitFrames;    /* Specify the initial number of frames for the interactive format */
    uint32_t Scale;         /* Data volume, video每桢的大小或者Audio的采样大小 */
    uint32_t Rate;          /* Scale/Rate=samples per second */
    uint32_t Start;         /* 数据流开始播放的Bit置，单Bit为Scale */
    uint32_t Length;        /* The amount of data in the data stream，单Bit为Scale */
    uint32_t RefBufSize;    /* Recommended buffer size */
    uint32_t Quality;       /* Decompression quality parameters，The greater the value，The better the quality */
    uint32_t SampleSize;    /* Audio sample size */
    struct                  /* videoframe所占的矩形 */
    {
        short Left;
        short Top;
        short Right;
        short Bottom;
    } Frame;
} STRH_HEADER;

/* BMP structure */
typedef struct
{
    uint32_t BmpSize;       /* bmp structure size, including (BmpSize) */
    long Width;             /* image width */
    long Height;            /* image height */
    uint16_t  Planes;       /* Number of planes，Must be1 */
    uint16_t  BitCount;     /* Number of pixel bits,0X0018express24Bit */
    uint32_t  Compression;  /* Compression type，for example:MJPG/H264wait */
    uint32_t  SizeImage;    /* Image size */
    long XpixPerMeter;      /* Horizontal resolution */
    long YpixPerMeter;      /* vertical resolution */
    uint32_t  ClrUsed;      /* The actual number of colors in the palette used,Not used in compressed format */
    uint32_t  ClrImportant; /* Important colors */
} BMP_HEADER;

/* Color table */
typedef struct
{
    uint8_t  rgbBlue;       /* brightness of blue(The value range is0-255) */
    uint8_t  rgbGreen;      /* Green brightness(The value range is0-255) */
    uint8_t  rgbRed;        /* brightness of red(The value range is0-255) */
    uint8_t  rgbReserved;   /* reserve，Must be0 */
} AVIRGBQUAD;

/* For strh, if it is a video stream, strf (stream format) makes the STRH_BMPHEADER block */
typedef struct
{
    uint32_t BlockID;       /* Block flag, strf==0X73747266 */
    uint32_t BlockSize;     /* piece大小(does not include the initial8byte,also就是BlockIDAndBlockSizeNot counted) */
    BMP_HEADER bmiHeader;   /* Bitmap information header */
    AVIRGBQUAD bmColors[1]; /* Color table */
} STRF_BMPHEADER;

/* forstrh,如果是Audio流,strf(Stream format)makeSTRH_WAVHEADERpiece */
typedef struct
{
    uint32_t BlockID;       /* Block flag, strf==0X73747266 */
    uint32_t BlockSize;     /* piece大小(does not include the initial8byte,also就是BlockIDAndBlockSizeNot counted) */
    uint16_t FormatTag;     /* Format logo:0X0001=PCM,0X0055=MP3 */
    uint16_t Channels;      /* Number of channels,Generally2,express立体声 */
    uint32_t SampleRate;    /* Audio sampling rate */
    uint32_t BaudRate;      /* Baud rate */
    uint16_t BlockAlign;    /* Data blocks对齐Logo */
    uint16_t Size;          /* The structure size */
} STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))
#define  MAKEDWORD(ptr)	(uint32_t)(((uint16_t)*(uint8_t*)(ptr)|(((uint16_t)*(uint8_t*)(ptr+1))<<8)|\
                               (((uint16_t)*(uint8_t*)(ptr+2))<<16)|(((uint16_t)*(uint8_t*)(ptr+3))<<24)))

AVISTATUS avi_init(uint8_t *buf, uint32_t size);                    /* initializationaviDecoder */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id);      /* FindID,IDMust be4个byte长度 */
AVISTATUS avi_get_streaminfo(uint8_t *buf);                         /* Get flow information */

#endif