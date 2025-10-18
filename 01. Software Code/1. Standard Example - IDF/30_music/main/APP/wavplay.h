/**
 ****************************************************************************************************
 * @file        wavplay.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       wavdecoding Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __WAVPLAY_H
#define __WAVPLAY_H

#include "audioplay.h"
#include "ff.h"
#include "es8388.h"
#include "xl9555.h"
#include "driver/i2s.h"
#include "driver/i2s_std.h"
#include "led.h"
#include "i2s.h"

#define WAV_TX_BUFSIZE    8192  /* definitionWAV TX DMA array size(play192Kbps@24bitWhen,Need to set up8192Not stuck if you are big) */

typedef struct
{
    uint32_t ChunkID;           /* chunk id;Here fixed"RIFF",Right now0X46464952 */
    uint32_t ChunkSize ;        /* Collection size;Total file size-8 */
    uint32_t Format;            /* Format;WAVE, i.e. 0X45564157 */
}ChunkRIFF;     /* RIFF block */

typedef struct
{
    uint32_t ChunkID;           /* chunk id;Here fixed"fmt ",Right now0X20746D66 */
    uint32_t ChunkSize ;        /* sonCollection size(Not includedIDandSize);Here is:20. */
    uint16_t AudioFormat;       /* Audio format; 0X01, represents linear PCM; 0X11, represents IMA ADPCM */
    uint16_t NumOfChannels;     /* Number of channels;1,Indicates mono;2,Indicates two channels; */
    uint32_t SampleRate;        /* Sampling rate; 0X1F40, indicating 8Khz */
    uint32_t ByteRate;          /* /Byte rate; */
    uint16_t BlockAlign;        /* Block alignment (byte); */
    uint16_t BitsPerSample;     /* oneindivualsamplingData size;4BitADPCM,Set as4 */
//    uint16_t ByteExtraData;   /* additional databyte;2indivual; LinearPCM,No thisindivualparameter */
}ChunkFMT;      /* fmt block */

typedef struct 
{
    uint32_t ChunkID;           /* chunk id; here is fixed as "fact", that is, 0X74636166; */
    uint32_t ChunkSize ;        /* sonCollection size(Not includedIDandSize);Here is:4. */
    uint32_t NumOfSamples;      /* The number of samples; */
}ChunkFACT;     /* fact block */

typedef struct 
{
    uint32_t ChunkID;           /* chunk id; here is fixed as "LIST", that is, 0X74636166; */
    uint32_t ChunkSize ;        /* sonCollection size(Not includedIDandSize);Here is:4. */
}ChunkLIST;     /* LIST block */

typedef struct
{
    uint32_t ChunkID;           /* chunk id;Here fixed"data",Right now0X5453494C */
    uint32_t ChunkSize ;        /* sonCollection size(Not includedIDandSize) */
}ChunkDATA;     /* Data block */

typedef struct
{ 
    ChunkRIFF riff;             /* riff block */
    ChunkFMT fmt;               /* fmt block */
//    ChunkFACT fact;           /* fact block LinearPCM,No thisindivualStructure */
    ChunkDATA data;             /* Data block */
}__WaveHeader;  /* wavhead */

typedef struct
{ 
    uint16_t audioformat;       /* Audio format; 0X01, represents linear PCM; 0X11, represents IMA ADPCM */
    uint16_t nchannels;         /* Number of channels;1,Indicates mono;2,Indicates two channels; */
    uint16_t blockalign;        /* Block alignment (byte); */
    uint32_t datasize;          /* WAVData size */

    uint32_t totsec ;           /* The duration of the entire song, unit: seconds */
    uint32_t cursec ;           /* Current playback time */

    uint32_t bitrate;           /* Bit rate (bit speed) */
    uint32_t samplerate;        /* Sampling rate */
    uint16_t bps;               /* Number of digits,for example16bit,24bit,32bit */

    uint32_t datastart;         /* The data frame startsBitSet(Offset inside the file) */
}__wavctrl;                     /* wav Playback control structure */ 

#define I2S_NUM                 (I2S_NUM_0)                         /* I2Sport */
#define I2S_BCK_IO              (GPIO_NUM_46)                       /* Set the serial clock pin, ES8388_SCLK */
#define I2S_WS_IO               (GPIO_NUM_9)                        /* Set the clock pins of left and right channels，ES8388_LRCK */
#define I2S_DO_IO               (GPIO_NUM_10)                       /* ES8388_SDOUT */
#define I2S_DI_IO               (GPIO_NUM_14)                       /* ES8388_SDIN */
#define IS2_MCLK_IO             (GPIO_NUM_3)                        /* ES8388_MCLK */
#define SAMPLE_RATE             (44100)                             /* Sampling rate */

uint8_t wav_decode_init(uint8_t *fname, __wavctrl *wavx);           /* WAVAnalytical Initialization */
uint8_t wav_play_song(uint8_t *fname);                              /* playcertainindivualWAVdocument */
void wavplay_i2s_init(int samplerate,int bits_sample);
size_t i2s_tx_write(uint8_t *buffer, uint32_t frame_size);
#endif