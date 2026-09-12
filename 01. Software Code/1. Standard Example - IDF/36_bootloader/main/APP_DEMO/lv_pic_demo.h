/**
 ****************************************************************************************************
 * @file        lv_pic_demo.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Photo album
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

#ifndef __LV_PIC_DEMO_H
#define __LV_PIC_DEMO_H

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
#include "bmp.h"
#include "jpeg.h"
#include "png.h"
#include "sdmmc_cmd.h"


/* PIC state */
enum PIC_STATE
{
    PIC_NULL,
    PIC_PAUSE,
    PIC_PLAY,
    PIC_NEXT,
    PIC_PREV
};

/* Function declarations */
void lv_pic_demo(void);

#endif
