/*
 * @Author: your name
 * @Date: 2021-10-13 17:25:16
 * @LastEditTime: 2022-03-10 16:55:39
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \S3_DEMO\16.Test\main\app_main.h
 */
/*** 
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-05-25 14:56:58
 * @LastEditors  : Kevincoooool
 * @LastEditTime : 2021-10-13 14:35:11
 * @FilePath     : \S3_LVGL\main\app_main.h
 */
#ifndef APP_MAIN_H
#define APP_MAIN_H
#if ESP_IDF_VERSION_MAJOR >= 5
	#define gpio_pad_select_gpio esp_rom_gpio_pad_select_gpio
#endif

#include <stdint.h>

#define APP_WIN_HEIGHT (lv_obj_get_height(lv_scr_act()))
#define APP_WIN_WIDTH  (lv_obj_get_width(lv_scr_act()))

#define VERSION "0.9.0"

typedef enum
{
    WAIT_FOR_WAKEUP,
    WAIT_FOR_CONNECT,
    START_DETECT,
    START_RECOGNITION,
    START_ENROLL,
    START_DELETE,

} en_fsm_state;

extern en_fsm_state g_state;

extern int g_is_enrolling;
extern int g_is_deleting;

#endif