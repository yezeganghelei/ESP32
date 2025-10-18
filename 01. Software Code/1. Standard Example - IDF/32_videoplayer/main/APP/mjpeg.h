/**
 ****************************************************************************************************
 * @file        mjpeg.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       MJPEGVideo processing code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

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
    struct jpeg_error_mgr pub;  /* Public field */
    jmp_buf setjmp_buffer;      /* Used to return */
};

extern int Windows_Width;       /* Image Width */
extern int Windows_Height;      /* image height */
#define rgb565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3))

/* Function declaration */
char mjpegdec_init(uint16_t offx, uint16_t offy);
void mjpegdec_free(void);
uint8_t mjpegdec_decode(uint8_t* buf, uint32_t bsize);

#endif