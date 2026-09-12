/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-13 17:17:24
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_calendar.c
 */
#include "page_calendar.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#include "button.h"
#include "page_start.h"

extern lv_obj_t *scr;
extern lv_obj_t *scr_body;
lv_obj_t *calendar;
extern lv_group_t *group_button;
extern struct _ksdiy_sys_t ksdiy_sys_t;
void ksdiy_calendar(void)
{
    calendar = lv_calendar_create(lv_scr_act(), NULL);
    lv_obj_set_size(calendar, LV_HOR_RES, LV_VER_RES);
    lv_obj_align(calendar, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_local_pad_all(calendar, LV_CALENDAR_PART_HEADER, LV_STATE_DEFAULT, LV_DPI / 32);
    lv_obj_set_style_local_pad_bottom(calendar, LV_CALENDAR_PART_DAY_NAMES, LV_STATE_DEFAULT, LV_DPI / 32);
    lv_obj_set_style_local_outline_pad(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, LV_DPI / 32);
    lv_obj_set_style_local_pad_all(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_text_font(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, lv_theme_get_font_small());

    /*Set today's date*/
    lv_calendar_date_t today;

    today.year = ksdiy_sys_t.timeinfo.tm_year+1900;
    today.month = ksdiy_sys_t.timeinfo.tm_mon+1;
    today.day = ksdiy_sys_t.timeinfo.tm_mday;
    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);
    // group_button=lv_group_create();
    lv_group_remove_all_objs(group_button);
    lv_group_add_obj(group_button, calendar); //Add to the decoder group
    lv_button_set_group(group_button);
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
			/*Long press OK to exit to the previous page*/
			// page.PagePop();
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
void page_calendar_load()
{
    ksdiy_calendar();
    obj_add_anim(
		calendar,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		APP_WIN_WIDTH,					  //Start value
		0,  //End value
		lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
	);
   
    ANIEND
		lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_touch);
}
static void Exit(void)
{
    obj_add_anim(
		calendar,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		0,					  //Start value
		APP_WIN_WIDTH,  //End value
		lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
	);
    ANIEND
    lv_obj_del(calendar);
}
static void Setup(void)
{
    //Get the available heap size
    printf("     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
    //Get the minimum free heap size ever
    printf("     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
    page_calendar_load();
}
void move_task_calendar(uint8_t move)
{

    switch (move)
    {

    case BT1_LONG: //Move up

        break;
    case BT1_LONGFREE: //Move up

        break;
    case BT3_LONG: //Move down

        break;
    case BT3_LONGFREE: //Move up


        break;
    case BT1_DOWN: //Move up
        encoder_handler(2);
        break;
    case BT3_DOWN: //Move down
        encoder_handler(3);
        break;
    default:
        break;
    }
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
void PageRegister_Calendar(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register Calendar with the page scheduler */\r\n");
}
