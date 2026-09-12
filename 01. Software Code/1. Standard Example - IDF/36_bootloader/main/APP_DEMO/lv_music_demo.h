/**
 ****************************************************************************************************
 * @file        lv_music.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Music player
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

#ifndef __LV_MUSIC_H
#define __LV_MUSIC_H

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
#include "lvgl_demo.h"
#include "sdmmc_cmd.h"


#define WAV_TX_BUFSIZE    8192  /* Define the WAV TX DMA array size (set it to 8192 to avoid stuttering when playing 192Kbps@24bit) */

typedef struct
{
    uint32_t ChunkID;           /* chunk id; fixed to "RIFF" here, i.e. 0X46464952 */
    uint32_t ChunkSize ;        /* Collection size; total file size - 8 */
    uint32_t Format;            /* Format; WAVE, i.e. 0X45564157 */
}ChunkRIFF;     /* RIFF chunk */

typedef struct
{
    uint32_t ChunkID;           /* chunk id; fixed to "fmt " here, i.e. 0X20746D66 */
    uint32_t ChunkSize ;        /* Sub-collection size (excluding ID and Size); here it is 20. */
    uint16_t AudioFormat;       /* Audio format; 0X01 = linear PCM, 0X11 = IMA ADPCM */
    uint16_t NumOfChannels;     /* Number of channels; 1 = mono, 2 = stereo */
    uint32_t SampleRate;        /* Sample rate; 0X1F40 = 8 kHz */
    uint32_t ByteRate;          /* Byte rate */
    uint16_t BlockAlign;        /* Block alignment (bytes) */
    uint16_t BitsPerSample;     /* Size of one sample; for 4-bit ADPCM, set to 4 */
//    uint16_t ByteExtraData;   /* Additional data bytes;2; linear PCM,no such parameter */
}ChunkFMT;      /* fmt chunk */

typedef struct 
{
    uint32_t ChunkID;           /* chunk id; fixed to "fact" here, i.e. 0X74636166; */
    uint32_t ChunkSize ;        /* Sub-collection size (excluding ID and Size); here it is 4. */
    uint32_t NumOfSamples;      /* Number of samples */
}ChunkFACT;     /* fact chunk */

typedef struct 
{
    uint32_t ChunkID;           /* chunk id; fixed to "LIST" here, i.e. 0X74636166; */
    uint32_t ChunkSize ;        /* Sub-collection size (excluding ID and Size); here it is 4. */
}ChunkLIST;     /* LIST chunk */

typedef struct
{
    uint32_t ChunkID;           /* chunk id; fixed to "data" here, i.e. 0X5453494C */
    uint32_t ChunkSize ;        /* Sub-collection size (excluding ID and Size) */
}ChunkDATA;     /* data chunk */

typedef struct
{ 
    ChunkRIFF riff;             /* RIFF chunk */
    ChunkFMT fmt;               /* fmt chunk */
//    ChunkFACT fact;           /* fact chunk linear PCM,no such structure */
    ChunkDATA data;             /* data chunk */
}__WaveHeader;  /* WAV header */

typedef struct
{ 
    uint16_t audioformat;       /* Audio format; 0X01 = linear PCM, 0X11 = IMA ADPCM */
    uint16_t nchannels;         /* Number of channels; 1 = mono, 2 = stereo */
    uint16_t blockalign;        /* Block alignment (bytes) */
    uint32_t datasize;          /* WAV data size */

    uint32_t totsec ;           /* Total song duration, in seconds */
    uint32_t cursec ;           /* Current playback duration */

    uint32_t bitrate;           /* Bitrate */
    uint32_t samplerate;        /* Sample rate */
    uint16_t bps;               /* Bit depth, e.g. 16-bit, 24-bit, 32-bit */

    uint32_t datastart;         /* Start position of the data frame (offset within the file) */
}__wavctrl;                     /* WAV playback control structure */ 

/* Music playback controller */
typedef struct
{
    uint8_t *tbuf;                          /* Temporary array, only needed for 24-bit decoding */
    FIL *file;                              /* Audio file pointer */

    uint8_t status;                         /* bit0: 0, pause playback; 1, resume playback */
                                            /* bit1: 0, stop playback; 1, start playback */
}__audiodev;

/* Music playback state */
enum MUSIC_STATE
{
    MUSIC_NULL,
    MUSIC_PAUSE,
    MUSIC_PLAY,
    MUSIC_NEXT,
    MUSIC_PREV
};

/* Function declarations */
void lv_music_demo(void);

#endif