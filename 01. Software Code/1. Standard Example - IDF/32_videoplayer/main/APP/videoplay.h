/**
 ******************************************************************************************************
 * @file videoplay.h
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief video player application code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 ******************************************************************************************************
 */

#ifndef __VIDEOPLAYER_H
#define __VIDEOPLAYER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "avi.h"
#include "xl9555.h"
#include "lcd.h"
#include "ff.h"
#include "esptim.h"
#include "mjpeg.h"
#include "es8388.h"
#include "driver/i2s.h"
#include "i2s.h"
#include "text.h"

/* Related definitions of cache space */
#define AVI_VIDEO_BUF_SIZE      (60 * 1024)

/* Declare function */
void video_play(void);  /* Play video */
uint8_t video_seek(FIL *favi, AVI_INFO *aviinfo, uint8_t *mbuf);
#endif