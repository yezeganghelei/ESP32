/**
 ****************************************************************************************************
 * @file        lvgl_demo.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LVGL V8 porting example
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
 
#ifndef __LVGL_DEMO_H
#define __LVGL_DEMO_H

#include "lcd.h"
#include "spi_sdcard.h"
#include "key.h"
#include "esp_timer.h"
#include "lvgl.h"
#include "demos/lv_demos.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t xGuiSemaphore;
extern uint32_t back_act_key;      /* Return-to-main-screen button */

/* Function declarations */
void lvgl_demo(void);       /* lvgl_demo entry function */

#endif
