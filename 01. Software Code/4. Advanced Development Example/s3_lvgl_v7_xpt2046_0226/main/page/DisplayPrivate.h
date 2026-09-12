/*
 * @Author: your name
 * @Date: 2021-11-13 14:29:15
 * @LastEditTime: 2021-11-15 15:24:56
 * @LastEditors: Please set LastEditors
 * @Description: Open koroFileHeader to view the configuration and settings: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: \s3_lvgl_v7\main\page\DisplayPrivate.h
 */

#ifndef __DISPLAYPRIVATE_H
#define __DISPLAYPRIVATE_H
/*Page*/
#include "PageManager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
/*LittleVGL*/
#include "lvgl.h"
void DisplayError_Init();
void Display_Init();
void Display_Update();



void PageDelay(uint32_t ms);
#define PageWaitUntil(condition)\
while(!(condition)){\
    lv_task_handler();\
}


#define LV_ANIM_TIME_DEFAULT 200
#define LV_SYMBOL_DEGREE_SIGN   "\xC2\xB0"

/**
  * @brief  Increment or decrement a variable; wrap to the minimum after exceeding the maximum, and to the maximum after dropping below the minimum
  * @param  src:controlled variable
  * @param  plus:value to add
  * @param  min:minimum value
  * @param  max:maximum value
  * @retval None
  */
#define __ValuePlus(src, plus, min, max) \
	do                                   \
	{                                    \
		int __value_temp = (src);        \
		__value_temp += (plus);          \
		if (__value_temp < (min))        \
			__value_temp = (max);        \
		else if (__value_temp > (max))   \
			__value_temp = (min);        \
		(src) = __value_temp;            \
	} while (0)

#define __Sizeof(arr) (sizeof(arr) / sizeof(arr[0]))


typedef enum
{
	Disp_Home = 1,	 //Home page
	Disp_Menu,		 //Menu selection screen
	Disp_Cam,		 //Camera display
	Disp_Color,		 //Color recognition
	Disp_Face,		 //Face recognition
	Disp_Baidu,		 //Baidu AI object and text recognition
	Disp_IMU,		 //Gyroscope
	Disp_FFT,		 //Music spectrum
	Disp_Daily,		 //Daily quote
	Disp_Game_2048,	 //2048 game
	Disp_Game_Snake, //Snake game
	Disp_Music,		 //Music player screen, based on the LVGL music demo
	Disp_Calendar,	 //Calendar
	Disp_Setting,	 //Settings screen; configure camera flip and air-conditioner type
	Disp_About,		 //About
	Disp_Max_Page,
} Display_Page; //Current screen enum


/*AppWindow*/
void AppWindow_Create();
lv_obj_t * AppWindow_GetCont(uint8_t pageID);
lv_coord_t AppWindow_GetHeight();
lv_coord_t AppWindow_GetWidth();
#define APP_WIN_HEIGHT AppWindow_GetHeight()
#define APP_WIN_WIDTH  AppWindow_GetWidth()
void Display_Init();
#endif
