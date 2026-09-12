/**
 ****************************************************************************************************
 * @file        avi.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       AVI video format parsing code
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
#define AVI_MAX_FRAME_SIZE  60 * 1024     /* Maximum frame size, cannot exceed 60KB */

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
#define AVI_HDRL_ID         0X6C726468      /* Info block tag */
#define AVI_MOVI_ID         0X69766F6D      /* Data blocks tag */
#define AVI_STRL_ID         0X6C727473      /* strl flag */

#define AVI_AVIH_ID         0X68697661      /* avih sub-chunk within AVI_HDRL_ID */
#define AVI_STRH_ID         0X68727473      /* strh (stream header) sub-chunk within AVI_STRL_ID */
#define AVI_STRF_ID         0X66727473      /* strf (stream format) sub-chunk within AVI_STRL_ID */
#define AVI_STRD_ID         0X64727473      /* strd sub-chunk within AVI_STRL_ID (optional) */

#define AVI_VIDS_STREAM     0X73646976      /* video stream */
#define AVI_AUDS_STREAM     0X73647561      /* audio stream */

#define AVI_VIDS_FLAG       0X6463          /* video stream tag */
#define AVI_AUDS_FLAG       0X7762          /* Audio stream tag */

#define AVI_FORMAT_MJPG     0X47504A4D

/* AVI information structure */
/* Add some important data,Store here,Convenient decoding */
typedef struct
{
    uint32_t SecPerFrame;       /* Video frame interval time (unit is us) */
    uint32_t TotalFrame;        /* Total number of frames in the file */
    uint32_t Width;             /* image width */
    uint32_t Height;            /* image height */
    uint32_t SampleRate;        /* Audio sampling rate */
    uint16_t Channels;          /* Number of channels, generally 2 for stereo */
    uint16_t AudioBufSize;      /* Audio buffer size */
    uint16_t AudioType;         /* Audio type: 0X0001=PCM; 0X0050=MP2; 0X0055=MP3; 0X2000=AC3; */
    uint16_t StreamID;          /* Stream type ID,StreamID=='dc'==0X6463 /StreamID=='wb'==0X7762 */
    uint32_t StreamSize;        /* Stream size,Must be an even number,If read an odd number,Add1.complement to even number */
    char *VideoFLAG;            /* Video frame marking,VideoFLAG="00dc"/"01dc" */
    char *AudioFLAG;            /* Audio frame tag,AudioFLAG="00wb"/"01wb" */
} AVI_INFO;

extern AVI_INFO g_avix;           /* aviFile related information */

/* AVI chunk information */
typedef struct
{
    uint32_t RiffID;            /* RiffID=='RIFF'==0X61766968 */
    uint32_t FileSize;          /* AVIFile size(does not include the initial8byte,alsoRIFFIDandFileSizeNot counted) */
    uint32_t AviID;             /* AviID=='AVI '==0X41564920 */
} AVI_HEADER;

/* AVI chunk information */
typedef struct
{
    uint32_t FrameID;           /* Frame ID,FrameID=='RIFF'==0X61766968 */
    uint32_t FrameSize;         /* frame size */
} FRAME_HEADER;

/* LIST chunk information */
typedef struct
{
    uint32_t ListID;            /* ListID=='LIST'==0X4c495354 */
    uint32_t BlockSize;         /* Chunk size (does not include the initial 8 bytes, i.e. ListID and BlockSize are not counted) */
    uint32_t ListType;          /* LIST sub-block type: hdrl (information block) / movi (data blocks) / idxl (index chunk, optional, not needed) */
} LIST_HEADER;

/* avih sub-chunk information */
typedef struct
{
    uint32_t BlockID;           /* Block logo:avih==0X61766968 */
    uint32_t BlockSize;         /* Block size (not including the initial 8 bytes, that is, BlockID and BlockSize are not counted) */
    uint32_t SecPerFrame;       /* Video frame interval time (unit is us) */
    uint32_t MaxByteSec;        /* Maximum data transmission rate,byte/Second */
    uint32_t PaddingGranularity;/* Granularity of data filling */
    uint32_t Flags;             /* Global tags of AVI files, such as whether they contain index blocks, etc. */
    uint32_t TotalFrame;        /* Total number of frames in the file */
    uint32_t InitFrames;        /* Initial frame count for the interactive format (non-interactive format should be 0) */
    uint32_t Streams;           /* Number of data stream types included, usually 2 */
    uint32_t RefBufSize;        /* Recommended read cache size for this file (should hold the largest chunk); default may be 1 Mbyte!!! */
    uint32_t Width;             /* image width */
    uint32_t Height;            /* image height */
    uint32_t Reserved[4];       /* reserve */
} AVIH_HEADER;

/* strh Streaming header block information(strh∈strl) */
typedef struct
{
    uint32_t BlockID;       /* Block logo:strh==0X73747268 */
    uint32_t BlockSize;     /* Block size (not including the initial 8 bytes, that is, BlockID and BlockSize are not counted) */
    uint32_t StreamType;    /* Data stream type: vids (0X73646976) = video; auds (0X73647561) = audio */
    uint32_t Handler;       /* Stream handler, i.e. the decoder for audio/video, e.g. MJPG/H264 */
    uint32_t Flags;         /* Flags: whether to allow output of this stream; whether the palette changes */
    uint16_t Priority;      /* Stream priority (when multiple streams of the same type exist, the highest priority is the default stream) */
    uint16_t Language;      /* Audio language code */
    uint32_t InitFrames;    /* Specify the initial number of frames for the interactive format */
    uint32_t Scale;         /* Data volume: video frame size or audio sample size */
    uint32_t Rate;          /* Scale/Rate=samples per second */
    uint32_t Start;         /* Bit position where the data stream starts playing; unit is Scale */
    uint32_t Length;        /* Amount of data in the data stream; unit is Scale */
    uint32_t RefBufSize;    /* Recommended buffer size */
    uint32_t Quality;       /* Decompression quality parameter; the larger the value, the better the quality */
    uint32_t SampleSize;    /* Audio sample size */
    struct                  /* Rectangle occupied by the video frame */
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
    uint16_t  Planes;       /* Number of planes, must be 1 */
    uint16_t  BitCount;     /* Number of pixel bits; 0X0018 means 24-bit */
    uint32_t  Compression;  /* Compression type, e.g. MJPG/H264 etc. */
    uint32_t  SizeImage;    /* Image size */
    long XpixPerMeter;      /* Horizontal resolution */
    long YpixPerMeter;      /* vertical resolution */
    uint32_t  ClrUsed;      /* The actual number of colors in the palette used,Not used in compressed format */
    uint32_t  ClrImportant; /* Important colors */
} BMP_HEADER;

/* Color table */
typedef struct
{
    uint8_t  rgbBlue;       /* Blue brightness (range 0-255) */
    uint8_t  rgbGreen;      /* Green brightness (range 0-255) */
    uint8_t  rgbRed;        /* Red brightness (range 0-255) */
    uint8_t  rgbReserved;   /* Reserved, must be 0 */
} AVIRGBQUAD;

/* For strh, if it is a video stream, strf (stream format) makes the STRH_BMPHEADER block */
typedef struct
{
    uint32_t BlockID;       /* Block flag, strf==0X73747266 */
    uint32_t BlockSize;     /* Chunk size (does not include the initial 8 bytes, i.e. BlockID and BlockSize are not counted) */
    BMP_HEADER bmiHeader;   /* Bitmap information header */
    AVIRGBQUAD bmColors[1]; /* Color table */
} STRF_BMPHEADER;

/* For strh: if it is an audio stream, strf (stream format) uses the STRH_WAVHEADER chunk */
typedef struct
{
    uint32_t BlockID;       /* Block flag, strf==0X73747266 */
    uint32_t BlockSize;     /* Chunk size (does not include the initial 8 bytes, i.e. BlockID and BlockSize are not counted) */
    uint16_t FormatTag;     /* Format logo:0X0001=PCM,0X0055=MP3 */
    uint16_t Channels;      /* Number of channels, generally 2 for stereo */
    uint32_t SampleRate;    /* Audio sampling rate */
    uint32_t BaudRate;      /* Baud rate */
    uint16_t BlockAlign;    /* Data block alignment tag */
    uint16_t Size;          /* The structure size */
} STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))
#define  MAKEDWORD(ptr)	(uint32_t)(((uint16_t)*(uint8_t*)(ptr)|(((uint16_t)*(uint8_t*)(ptr+1))<<8)|\
                               (((uint16_t)*(uint8_t*)(ptr+2))<<16)|(((uint16_t)*(uint8_t*)(ptr+3))<<24)))

AVISTATUS avi_init(uint8_t *buf, uint32_t size);                    /* initializationaviDecoder */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id);      /* Find ID; ID must be 4 bytes long */
AVISTATUS avi_get_streaminfo(uint8_t *buf);                         /* Get flow information */

#endif