/**
 ****************************************************************************************************
 * @file        audioplay.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Music player Application Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __AUDIOPLAY_H
#define __AUDIOPLAY_H

#include "ff.h"
#include "wavplay.h"
#include "exfuns.h"
#include "i2s.h"
#include "lcd.h"
#include "text.h"

/* Music Playback Controller */
typedef struct
{
    uint8_t *tbuf;                          /* Temporary array,Only in24bitNeeded when decoding */
    FIL *file;                              /* Audio file pointer */

    uint8_t status;                         /* bit0:0,Pause playback;1,Continue playing */
                                            /* bit1:0, end playback; 1, start playback */
}__audiodev;

extern __audiodev g_audiodev;               /* Music Playback Controller */

/******************************************************************************************/

void wav_sai_dma_callback(void);
void audio_start(void);                                                     /* Start audio playback */
void audio_stop(void);                                                      /* Stop audio playback */
uint16_t audio_get_tnum(uint8_t *path);                                     /* Get the total number of target files under the path path */
void audio_index_show(uint16_t index, uint16_t total);                      /* Show track index */
void audio_msg_show(uint32_t totsec, uint32_t cursec, uint32_t bitrate);    /* Show playback time,Bit rate information */
void audio_play(void);                                                      /* Play music */
uint8_t audio_play_song(uint8_t *fname);                                    /* Play an audio file */

#endif