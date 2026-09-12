/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-19 15:37:55
 * @FilePath: \s3_lvgl_v7\main\page\page_music.c
 */

#include "page_start.h"
#include "page_menu.h"
#include "app_main.h"
#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#include "lv_gif.h"

/*This page's window*/
static lv_obj_t *appWindow;

/**
 * @descripttion: Create the title bar
 * @param {*}
 * @return {*}
 */
static void Title_Create()
{

	appWindow = lv_cont_create(lv_scr_act(), NULL);
	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //Set the screen background
	lv_obj_add_style(appWindow, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_pos(appWindow, 0, 0);
	lv_obj_set_size(appWindow, APP_WIN_WIDTH, APP_WIN_HEIGHT);
}
static void event_handler_touch(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_GESTURE)
	{

		switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
		{
		case LV_GESTURE_DIR_TOP:

			printf("LV_GESTURE_DIR_TOP.\n\r");
			break;
		case LV_GESTURE_DIR_BOTTOM:
			printf("LV_GESTURE_DIR_BOTTOM.\n\r");
			break;
		case LV_GESTURE_DIR_RIGHT:
			printf("LV_GESTURE_DIR_RIGHT.\n\r");
			break;
		case LV_GESTURE_DIR_LEFT:
			printf("LV_GESTURE_DIR_LEFT.\n\r");
			break;
		default:
			break;
		}
	}
	switch (event)
	{
	case LV_EVENT_LONG_PRESSED: /* Long press */
		page.PagePop();
		printf("Long press\n");
		break;
	}

	/*Etc.*/
}
static void Exit(void)
{
	my_demo_music_end(appWindow);
	obj_add_anim(
		appWindow,						   //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		0,								   //Start value
		APP_WIN_WIDTH,					   //End value
		lv_anim_path_linear				   //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	lv_obj_del(appWindow);
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
}

static void Setup(void)
{
	//Get the available heap size

	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));

	Title_Create();

	obj_add_anim(
		appWindow,						   //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		APP_WIN_WIDTH,					   //Start value
		0,								   //End value
		lv_anim_path_linear				   //Animation effect: simulate a bouncing object falling
	);

	ANIEND
	my_demo_music_start(appWindow);
}
/**
  * @brief  Page event
  * @param  btn:button that raised the event
  * @param  event:event ID
  * @retval None
  */
static void Event(void *btn, int event)
{
}

/**
  * @brief  Page registration
  * @param  pageID:ID assigned to this page
  * @retval None
  */
void PageRegister_Music(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register Music with the page scheduler */\r\n");
}
