/**
 ****************************************************************************************************
 * @file        lv_video.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Video player
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __LV_VIDEO_H
#define __LV_VIDEO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "exfuns.h"
#include "ff.h"
#include "driver/i2s.h"
#include "driver/i2s_std.h"
#include "i2s.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "mjpeg.h"
#include "sdmmc_cmd.h"
#include "esptim.h"


/* Frame size; set it according to your memory allocation.
 * If there is plenty of memory, set it larger; if memory is limited, set it smaller.
 * Generally, set this value equal to our frame buffer size.
 */
#define AVI_MAX_FRAME_SIZE  60 * 1024     /* Maximum frame size, must not exceed 60KB */

/* Error types */
typedef enum
{
    AVI_OK = 0,             /* 0, success */
    AVI_RIFF_ERR,           /* 1, RIFF ID read failed */
    AVI_AVI_ERR,            /* 2, AVI ID read failed */
    AVI_LIST_ERR,           /* 3, LIST ID read failed */
    AVI_HDRL_ERR,           /* 4, HDRL ID read failed */
    AVI_AVIH_ERR,           /* 5, AVIH ID read failed */
    AVI_STRL_ERR,           /* 6, STRL ID read failed */
    AVI_STRH_ERR,           /* 7, STRH ID read failed */
    AVI_STRF_ERR,           /* 8, STRF ID read failed */
    AVI_MOVI_ERR,           /* 9, MOVI ID read failed */
    AVI_FORMAT_ERR,         /* 10, format error */
    AVI_STREAM_ERR,         /* 11, stream error */
} AVISTATUS;

#define AVI_RIFF_ID         0X46464952
#define AVI_AVI_ID          0X20495641
#define AVI_LIST_ID         0X5453494C
#define AVI_HDRL_ID         0X6C726468      /* Information block flag */
#define AVI_MOVI_ID         0X69766F6D      /* Data block flag */
#define AVI_STRL_ID         0X6C727473      /* strl flag */

#define AVI_AVIH_ID         0X68697661      /* avih sub-chunk in AVI_HDRL_ID */
#define AVI_STRH_ID         0X68727473      /* strh (stream header) sub-chunk in AVI_STRL_ID */
#define AVI_STRF_ID         0X66727473      /* strf (stream format) sub-chunk in AVI_STRL_ID */
#define AVI_STRD_ID         0X64727473      /* strd sub-chunk in AVI_STRL_ID (optional) */

#define AVI_VIDS_STREAM     0X73646976      /* Video stream */
#define AVI_AUDS_STREAM     0X73647561      /* Audio stream */


#define AVI_VIDS_FLAG       0X6463          /* Video stream flag */
#define AVI_AUDS_FLAG       0X7762          /* Audio stream flag */

#define AVI_FORMAT_MJPG     0X47504A4D


/* AVI information structure */
/* Store important data here to facilitate decoding */
typedef struct
{
    uint32_t SecPerFrame;       /* Video frame interval (unit: us) */
    uint32_t TotalFrame;        /* Total number of frames */
    uint32_t Width;             /* Image width */
    uint32_t Height;            /* Image height */
    uint32_t SampleRate;        /* Audio sample rate */
    uint16_t Channels;          /* Number of channels, usually 2 for stereo */
    uint16_t AudioBufSize;      /* Audio buffer size */
    uint16_t AudioType;         /* Audio type: 0X0001=PCM; 0X0050=MP2; 0X0055=MP3; 0X2000=AC3; */
    uint16_t StreamID;          /* Stream type ID, StreamID=='dc'==0X6463 / StreamID=='wb'==0X7762 */
    uint32_t StreamSize;        /* Stream size, must be even; if an odd value is read, add 1 to make it even */
    char *VideoFLAG;            /* Video frame flag, VideoFLAG="00dc"/"01dc" */
    char *AudioFLAG;            /* Audio frame flag, AudioFLAG="00wb"/"01wb" */
} AVI_INFO;

extern AVI_INFO g_avix;           /* AVI file-related information */

/* AVI chunk information */
typedef struct
{
    uint32_t RiffID;            /* RiffID=='RIFF'==0X61766968 */
    uint32_t FileSize;          /* AVI file size (excluding the first 8 bytes; RIFFID and FileSize are not counted) */
    uint32_t AviID;             /* AviID=='AVI '==0X41564920 */
} AVI_HEADER;

/* AVI chunk information */
typedef struct
{
    uint32_t FrameID;           /* Frame ID, FrameID=='RIFF'==0X61766968 */
    uint32_t FrameSize;         /* Frame size */
} FRAME_HEADER;


/* LIST chunk information */
typedef struct
{
    uint32_t ListID;            /* ListID=='LIST'==0X4c495354 */
    uint32_t BlockSize;         /* Block size (excluding the first 8 bytes; ListID and BlockSize are not counted) */
    uint32_t ListType;          /* LIST sub-chunk type: hdrl (information)/movi (data)/idxl (index, optional) */
} LIST_HEADER;

/* avih sub-chunk information */
typedef struct
{
    uint32_t BlockID;           /* Block flag: avih==0X61766968 */
    uint32_t BlockSize;         /* Block size (excluding the first 8 bytes, i.e. BlockID and BlockSize are not counted) */
    uint32_t SecPerFrame;       /* Video frame interval (unit: us) */
    uint32_t MaxByteSec;        /* Maximum data transfer rate, bytes/second */
    uint32_t PaddingGranularity;/* Data padding granularity */
    uint32_t Flags;             /* Global AVI file flags, e.g. whether it contains an index chunk */
    uint32_t TotalFrame;        /* Total number of frames */
    uint32_t InitFrames;        /* Initial frame count for interactive formats (0 for non-interactive formats) */
    uint32_t Streams;           /* Number of stream types, usually 2 */
    uint32_t RefBufSize;        /* Suggested buffer size for reading this file (should hold the largest chunk); default may be 1 MB!!! */
    uint32_t Width;             /* Image width */
    uint32_t Height;            /* Image height */
    uint32_t Reserved[4];       /* Reserved */
} AVIH_HEADER;

/* strh stream header sub-chunk information (strh in strl) */
typedef struct
{
    uint32_t BlockID;       /* Block flag: strh==0X73747268 */
    uint32_t BlockSize;     /* Block size (excluding the first 8 bytes, i.e. BlockID and BlockSize are not counted) */
    uint32_t StreamType;    /* Stream type, vids (0X73646976): video; auds (0X73647561): audio */
    uint32_t Handler;       /* Stream handler; for audio/video this is the decoder, e.g. MJPG/H264 */
    uint32_t Flags;         /* Flags: whether this stream may output? does the palette change? */
    uint16_t Priority;      /* Stream priority (when multiple streams of the same type exist, the highest priority is the default) */
    uint16_t Language;      /* Audio language code */
    uint32_t InitFrames;    /* Initial frame count for interactive formats */
    uint32_t Scale;         /* Data amount: video frame size or audio sample size */
    uint32_t Rate;          /* Scale/Rate = samples per second */
    uint32_t Start;         /* Start position of the data stream, in Scale units */
    uint32_t Length;        /* Data amount of the stream, in Scale units */
    uint32_t RefBufSize;    /* Suggested buffer size */
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
    uint32_t BmpSize;       /* BMP structure size, including BmpSize itself */
    long Width;             /* Image width */
    long Height;            /* Image height */
    uint16_t  Planes;       /* Number of planes, must be 1 */
    uint16_t  BitCount;     /* Bits per pixel; 0X0018 = 24-bit */
    uint32_t  Compression;  /* Compression type, e.g. MJPG/H264 */
    uint32_t  SizeImage;    /* Image size */
    long XpixPerMeter;      /* Horizontal resolution */
    long YpixPerMeter;      /* Vertical resolution */
    uint32_t  ClrUsed;      /* Number of palette colors actually used; unused in compressed formats */
    uint32_t  ClrImportant; /* Important colors */
} BMP_HEADER;

/* Color table */
typedef struct
{
    uint8_t  rgbBlue;       /* Blue intensity (range 0-255) */
    uint8_t  rgbGreen;      /* Green intensity (range 0-255) */
    uint8_t  rgbRed;        /* Red intensity (range 0-255) */
    uint8_t  rgbReserved;   /* Reserved, must be 0 */
} AVIRGBQUAD;

/* For strh, if it is a video stream, strf (stream format) uses the STRH_BMPHEADER chunk */
typedef struct
{
    uint32_t BlockID;       /* Block flag, strf==0X73747266 */
    uint32_t BlockSize;     /* Block size (excluding the first 8 bytes, i.e. BlockID and BlockSize are not counted) */
    BMP_HEADER bmiHeader;   /* Bitmap information header */
    AVIRGBQUAD bmColors[1]; /* Color table */
} STRF_BMPHEADER;

/* For strh, if it is an audio stream, strf (stream format) uses the STRH_WAVHEADER chunk */
typedef struct
{
    uint32_t BlockID;       /* Block flag, strf==0X73747266 */
    uint32_t BlockSize;     /* Block size (excluding the first 8 bytes, i.e. BlockID and BlockSize are not counted) */
    uint16_t FormatTag;     /* Format flag: 0X0001=PCM, 0X0055=MP3 */
    uint16_t Channels;      /* Number of channels, usually 2 for stereo */
    uint32_t SampleRate;    /* Audio sample rate */
    uint32_t BaudRate;      /* Baud rate */
    uint16_t BlockAlign;    /* Data block alignment flag */
    uint16_t Size;          /* Size of this structure */
} STRF_WAVHEADER;

#define	 MAKEWORD(ptr)	(uint16_t)(((uint16_t)*((uint8_t*)(ptr))<<8)|(uint16_t)*(uint8_t*)((ptr)+1))
#define  MAKEDWORD(ptr)	(uint32_t)(((uint16_t)*(uint8_t*)(ptr)|(((uint16_t)*(uint8_t*)(ptr+1))<<8)|\
                               (((uint16_t)*(uint8_t*)(ptr+2))<<16)|(((uint16_t)*(uint8_t*)(ptr+3))<<24)))


/* Video playback state */
enum VIDEO_STATE
{
    VIDEO_NULL,
    VIDEO_PAUSE,
    VIDEO_PLAY,
    VIDEO_NEXT,
    VIDEO_PREV
};


/* Function declarations */
AVISTATUS avi_init(uint8_t *buf, uint32_t size);                    /* Initialize the AVI decoder */
uint32_t avi_srarch_id(uint8_t *buf, uint32_t size, char *id);      /* Find an ID; the ID must be 4 bytes long */
AVISTATUS avi_get_streaminfo(uint8_t *buf);                         /* Get stream information */
void lv_video_demo(void);

#endif