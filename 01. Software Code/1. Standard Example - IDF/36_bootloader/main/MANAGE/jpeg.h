/**
 ****************************************************************************************************
 * @file        jpeg.h
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding - JPEG/JPG decoder
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

#ifndef __JPEG_H
#define __JPEG_H

#include <string.h>
#include <unistd.h>
#include "esp_system.h"
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "ff.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pngle.h"
#include "tjpgd.h"
#include "mjpeg.h"


/* rgb565 format */
typedef uint16_t pixel_jpeg;

/* Function declarations */
esp_err_t decode_jpeg(pixel_jpeg ***pixels, char * file, int screenWidth, int screenHeight, int * imageWidth, int * imageHeight);
esp_err_t release_image(pixel_jpeg ***pixels, int screenWidth, int screenHeight);
TickType_t jpeg_decode(const char *filename, int width, int height,lcd_write_cb lcd_cb); /* JPEG decoding */

#endif
