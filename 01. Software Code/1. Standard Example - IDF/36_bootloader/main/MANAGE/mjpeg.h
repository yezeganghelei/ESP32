/**
 ****************************************************************************************************
 * @file        mjpeg.h
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       MJPEG video processing
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
 
#ifndef __MJPEG_H
#define __MJPEG_H 

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "jpeglib.h"
#include "lcd.h"


struct my_error_mgr
{
    struct jpeg_error_mgr pub;  /* Public fields */
    jmp_buf setjmp_buffer;      /* Used for returning */
};

extern int Windows_Width;       /* Image width */
extern int Windows_Height;      /* Image height */
#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

typedef void (*lcd_write_cb)(uint32_t w,uint32_t h,uint8_t *video_buf);

/* Function declarations */
char mjpegdec_init(uint16_t offx, uint16_t offy);
void mjpegdec_free(void);
uint8_t mjpegdec_decode(uint8_t* buf, uint32_t bsize,lcd_write_cb lcd_cb);
void mjpegdec_malloc(void);
void mjpegdec_video_free(void);
#endif
