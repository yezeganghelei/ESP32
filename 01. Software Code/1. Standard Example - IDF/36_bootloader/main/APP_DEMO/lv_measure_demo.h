/**
 ****************************************************************************************************
 * @file        lv_measure_demo.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-11-04
 * @brief       Measurement system
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

#ifndef __LV_MEASURE_DEMO_H
#define __LV_MEASURE_DEMO_H

#include "lvgl.h"
#include "ff.h"
#include "lcd.h"
#include "app_ui.h"
#include "sensor.h"
#include "driver/temperature_sensor.h"


/* Measurement structure */
typedef struct
{
    lv_obj_t *lv_main_cont;
    
   struct
   {
        lv_obj_t *lv_small_cont;
        struct
        {
            lv_timer_t * lv_ap3216c_timer;
            lv_obj_t * ap_ir;
            lv_obj_t * ap_ps;
            lv_obj_t * ap_als;
            lv_obj_t * ap_ir_label;
            lv_obj_t * ap_ir_unit;
            lv_obj_t * ap_ps_label;
            lv_obj_t * ap_ps_unit;
            lv_obj_t * ap_als_label;
            lv_obj_t * ap_als_unit;
        }ap3216c;
            
        struct
        {
            lv_timer_t * lv_temp_timer;
            lv_obj_t * temp_obj;
            lv_obj_t * temp_label;
            lv_obj_t * temp_unit;
            lv_obj_t * temp_slider;
            lv_obj_t * temp_line1;
            lv_obj_t * temp_line1_1;
            lv_obj_t * temp_line2;
            lv_obj_t * temp_line2_1;
        }temp;
   }small_cont;
}measure_obj_t;

/* Function declarations */
void lv_measure_demo(void);

#endif
