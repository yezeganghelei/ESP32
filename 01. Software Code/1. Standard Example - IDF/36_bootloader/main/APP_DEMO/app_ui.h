/**
 ****************************************************************************************************
 * @file        app_ui.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-08-01
 * @brief       ESP32-S3 comprehensive example UI
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

#ifndef __APP_DEMO_H
#define __APP_DEMO_H

#include "xl9555.h"
#include "manage.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "24cxx.h"
#include "adc1.h"
#include "ap3216c.h"
#include "spi_sdcard.h"
#include "qma6100p.h"
#include "es8388.h"
#include "i2s.h"
#include "tinyusb.h"
#include "lvgl.h"
#include "image.h"
#include "lv_file_demo.h"
#include "lv_camera_demo.h"
#include "lv_video_demo.h"
#include "lv_music_demo.h"
#include "lv_pic_demo.h"
#include "lv_clock_demo.h"
#include "lv_measure_demo.h"
#include "esp_rtc.h"


#define APP_NUM    9

/* APP icon description information */
typedef struct
{
    char* app_text_English;
    char* app_text_Chinese;
    const void* app_image;
}app_image_info;

/* APP icon description information */
typedef struct
{
    uint8_t image_flag;
    lv_img_dsc_t image_bin[APP_NUM];
    lv_obj_t *background;
    lv_img_dsc_t image_background_bin;
    lv_obj_t *app_btn[APP_NUM];
    lv_obj_t *app_name[APP_NUM];
    lv_obj_t *lv_main_cont;
    
   struct
   {
        lv_timer_t* lv_rtc_timer;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
        uint8_t year;
        uint8_t month;
        uint8_t date;
        uint8_t week;
        char rtc_tbuf[40];
   }rtc;
    
}app_obj_t;

/* Return controller (can be used to return to the main screen) */
typedef struct
{
    lv_obj_t* current_parent;
    lv_obj_t* fouc_parent;
    lv_obj_t* del_parent;
    void (*Function)(void);     /* Function to delete all widgets on the screen */
} lv_m_general;

extern lv_m_general app_obj_general;
extern app_obj_t app_obj_btn;
extern lv_group_t *ctrl_g;
/* Function declarations */
void lv_load_main_window(void);
void lv_msgbox(char *name);
void lv_app_show(void);
void lv_app_del(void);
#endif
