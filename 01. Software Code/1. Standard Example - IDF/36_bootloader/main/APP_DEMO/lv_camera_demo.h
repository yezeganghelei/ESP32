/**
 ****************************************************************************************************
 * @file        lv_camera_demo.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Camera example
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

#ifndef __LV_CAMERA_DEMO_H
#define __LV_CAMERA_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "camera.h"
#include "esp_camera.h"
#include "mjpeg.h"
#include "lv_file_demo.h"


typedef struct
{
    lv_obj_t *lv_camera_cont;   /* Camera main container */

    struct
    {
        lv_obj_t* camera_header;
    }camera_buf;

}lv_camera_struct;

/* Function declarations */
void lv_camera_demo(void);

#endif
