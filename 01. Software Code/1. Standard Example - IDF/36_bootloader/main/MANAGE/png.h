/**
 ****************************************************************************************************
 * @file        png.h
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image decoding - PNG decoder
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

#ifndef __PNG_H
#define __PNG_H

#include <stdint.h>
#include <stdbool.h>
#include "pngle.h"
#include "ff.h"
#include "esp_log.h"
#include "mjpeg.h"


/* Function declarations */
void png_init(pngle_t *pngle, uint32_t w, uint32_t h);
void png_draw(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t rgba[4]);
void png_finish(pngle_t *pngle);
TickType_t png_decode(const char *filename, int width, int height,lcd_write_cb lcd_cb);

#endif
