/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-07-02 09:59:37
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-07-17 12:05:06
 * @FilePath: \SP_V2_DEMO\s3_lvgl_v7_xpt2046_0226\main\page\page_menu.c
 */
#include "page_menu.h"
#include "page_start.h"
#include "page_cam.h"

#include "page_calendar.h"

#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "app_anim.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"

static void ICON_Grp_Move(int8_t dir);

/*This page's window*/
static lv_obj_t *appWindow;
/*Title bar*/
static lv_obj_t *labelTitle;
/*Title bar separator line*/
static lv_obj_t *lineTitle;
/*Icon display container, used for clipped display*/
static lv_obj_t *contDisp;
/*Icon group container*/
static lv_obj_t *contICON;

/*Menu switch speed*/
static uint16_t anim_speed = 200;
/*Target zoom value for the selected icon*/
static uint16_t anim_target_zoom = 400;
/*Target y coordinate for the selected icon*/
static uint16_t anim_target_y = 60;
/*Target y coordinate for the selected icon*/
uint16_t anim_start_y = 130;

/*Icon spacing*/
static uint8_t ICON_IntervalPixel = 30;
// uint16_t ICON_IntervalPixel = (uint16_t)(DISP_MAX_VER / 6);
/*Icon size*/
static const uint8_t ICON_Size = 20;
/*Index of the currently selected icon*/
static int8_t ICON_NowSelIndex = 0;
/*Offset correction for each icon*/
static int8_t ICON_Offset = 20;
/*Maximum icon index*/
#define ICON_MAX_INDEX (__Sizeof(ICON_Grp) - 1)

//Image declarations
LV_IMG_DECLARE(imgcam);
LV_IMG_DECLARE(imgcolor);
LV_IMG_DECLARE(imgface);
LV_IMG_DECLARE(imgcloud);
LV_IMG_DECLARE(imgimu);
LV_IMG_DECLARE(imgspectrum);
LV_IMG_DECLARE(imgheart);
LV_IMG_DECLARE(img2048);
LV_IMG_DECLARE(imgsnake);
LV_IMG_DECLARE(imgmusic);
LV_IMG_DECLARE(imgcalendar);
LV_IMG_DECLARE(imgset);
LV_IMG_DECLARE(imgabout);
static ICON_TypeDef ICON_Grp[] =
	{
		{.img_buffer = &imgcam, .text = "Camera", .pageID = Disp_Cam},
		{.img_buffer = &imgcolor, .text = "Color", .pageID = Disp_Color},
		{.img_buffer = &imgface, .text = "Face", .pageID = Disp_Face},
		{.img_buffer = &imgcloud, .text = "Baidu AI", .pageID = Disp_Baidu},
		{.img_buffer = &imgimu, .text = "IMU", .pageID = Disp_IMU},
		{.img_buffer = &imgspectrum, .text = "Spectrum", .pageID = Disp_FFT},
		{.img_buffer = &imgheart, .text = "Daily", .pageID = Disp_Daily},
		{.img_buffer = &img2048, .text = "2048", .pageID = Disp_Game_2048},
		{.img_buffer = &imgsnake, .text = "Snake", .pageID = Disp_Game_Snake},
		{.img_buffer = &imgmusic, .text = "Music", .pageID = Disp_Music},
		{.img_buffer = &imgcalendar, .text = "Calendar", .pageID = Disp_Calendar},
		{.img_buffer = &imgset, .text = "Setting", .pageID = Disp_Setting},
		{.img_buffer = &imgabout, .text = "About", .pageID = Disp_About},
};
static void event_handler_page_menu(lv_obj_t *obj, lv_event_t event)
{
	if (event == LV_EVENT_GESTURE)
	{

		switch (lv_indev_get_gesture_dir(lv_indev_get_act()))
		{
		case LV_GESTURE_DIR_TOP:

			//printf("LV_GESTURE_DIR_TOP.\n\r");
			break;
		case LV_GESTURE_DIR_BOTTOM:
			printf("LV_GESTURE_DIR_BOTTOM.\n\r");
			/*Long press OK to exit to the previous page*/
			page.PagePop();
			break;
		case LV_GESTURE_DIR_RIGHT:
			ICON_Grp_Move(-1);
			//printf("LV_GESTURE_DIR_RIGHT.\n\r");
			break;
		case LV_GESTURE_DIR_LEFT:
			ICON_Grp_Move(1);
			//printf("LV_GESTURE_DIR_LEFT.\n\r");
			break;
		default:
			break;
		}
	}

	switch (event)
	{
	case LV_EVENT_PRESSED: /* Pressed */
		printf("Pressed\n");
		break;

	case LV_EVENT_SHORT_CLICKED: /* Short click */
		printf("Short clicked\n");
		break;

	case LV_EVENT_CLICKED: /* Click */
		/*Click OK to enter the corresponding page*/

		printf("Clicked\n");
		break;

	case LV_EVENT_LONG_PRESSED: /* Long press */
		if (ICON_Grp[ICON_NowSelIndex].pageID != 0)
		{
			page.PagePush(ICON_Grp[ICON_NowSelIndex].pageID);
		}
		printf("Long press\n");
		break;

	case LV_EVENT_LONG_PRESSED_REPEAT: /* Long press repeat */
		printf("Long press repeat\n");
		break;

	case LV_EVENT_RELEASED: /* Released */

		printf("Released\n");
		break;
	}

	/*Etc.*/
}

//What the MOVE task should do when the current screen is the menu
void move_task_menu(uint8_t dir)
{

	switch (dir)
	{
	case BT1_DOWN: //Move up
		ICON_Grp_Move(-1);
		break;
	case BT1_LONG: //Move up
		ICON_Grp_Move(-1);
		break;
	case BT1_LONGFREE: //Move up
		break;
	case BT3_DOWN: //Move down
		ICON_Grp_Move(1);
		break;
	case BT3_LONG: //Move down
		ICON_Grp_Move(1);
		break;
	case BT3_LONGFREE: //Move up
		break;

	default:
		break;
	}
}

/**
  * @brief  Create the title bar
  * @param  None
  * @retval None
  */
static void Title_Create()
{
	ICON_IntervalPixel = (uint16_t)(DISP_MAX_VER / 3);
	ICON_Offset = ICON_IntervalPixel / 2;
	LV_FONT_DECLARE(number);
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
	lv_obj_set_pos(appWindow, 0, APP_WIN_HEIGHT);
	lv_obj_set_size(appWindow, APP_WIN_WIDTH, APP_WIN_HEIGHT);

	labelTitle = lv_label_create(appWindow, NULL);

	static lv_style_t style_label;
	lv_style_init(&style_label);

	lv_style_set_radius(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_text_font(labelTitle, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &number);
	lv_style_set_text_letter_space(&style_label, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(labelTitle, LV_LABEL_PART_MAIN, &style_label);
	lv_label_set_long_mode(labelTitle, LV_LABEL_LONG_BREAK);
	lv_label_set_align(labelTitle, LV_LABEL_ALIGN_CENTER);
	lv_obj_set_pos(labelTitle, 0, 0);
	lv_obj_set_size(labelTitle, APP_WIN_WIDTH, 60);
	lv_label_set_recolor(labelTitle, true);

	/*The second icon is selected by default*/
	lv_label_set_static_text(labelTitle, ICON_Grp[0].text);
	lv_obj_align(labelTitle, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
	lv_obj_set_auto_realign(labelTitle, true);

	lineTitle = lv_line_create(appWindow, NULL);
	static lv_style_t style_line;
	lv_style_init(&style_line);
	lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, lv_color_make(0xFa, 0x05, 0x05));
	lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 5);
	lv_obj_add_style(lineTitle, LV_LINE_PART_MAIN, &style_line);
	lv_obj_set_pos(lineTitle, 0, 55);
	lv_obj_set_size(lineTitle, DISP_MAX_HOR, 5);

	static lv_point_t screen_line3[] = {{0, 0}, {LV_HOR_RES_MAX, 0}};
	lv_line_set_points(lineTitle, screen_line3, 2);
}

//Create the menu screen
static void Cont_create(void)
{
	uint8_t i;
	contDisp = lv_cont_create(appWindow, NULL);
	static lv_style_t style_cont;
	lv_style_set_pad_left(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //Set the screen background
	lv_obj_add_style(contDisp, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_size(contDisp, APP_WIN_WIDTH, DISP_MAX_VER - 60);
	lv_obj_set_pos(contDisp, 0, 60);

	contICON = lv_cont_create(contDisp, NULL);
	static lv_style_t style_cont1;
	lv_style_set_pad_left(&style_cont1, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(&style_cont1, LV_STATE_DEFAULT, 255);
	lv_style_set_pad_top(&style_cont1, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont1, LV_STATE_DEFAULT, LV_COLOR_RED);
	lv_style_set_border_width(&style_cont1, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont1, LV_STATE_DEFAULT, 255);
	lv_style_set_bg_color(&style_cont1, LV_STATE_DEFAULT, LV_COLOR_RED); //Set the screen background

	lv_obj_add_style(contICON, LV_BTN_PART_MAIN, &style_cont);
	lv_obj_set_size(contICON, (ICON_Size + ICON_IntervalPixel) * __Sizeof(ICON_Grp) + 50, lv_obj_get_height(contDisp));
	lv_obj_set_x(contICON, lv_obj_get_width(contDisp));

	for (i = 0; i < __Sizeof(ICON_Grp); i++)
	{
		ICON_Grp[i].img = lv_img_create(contICON, NULL);			//Create an image to display the menu
		lv_img_set_src(ICON_Grp[i].img, ICON_Grp[i].img_buffer);	//Set the displayed image
		lv_obj_align(ICON_Grp[i].img, NULL, LV_ALIGN_CENTER, 0, 0); //Set the image to the top
		/*Calculate the offset*/
		lv_obj_set_x(ICON_Grp[i].img, (ICON_Size + ICON_IntervalPixel) * i + ICON_Offset);
	}
}
/**
  * @brief  Create the scroll wheel shadow
  * @param  None
  * @retval None
  */
static void ImgShadow_Create()
{
	LV_IMG_DECLARE(ImgShadowUp);
	LV_IMG_DECLARE(ImgShadowDown);

	lv_obj_t *imgUp = lv_img_create(contDisp, NULL);
	lv_img_set_src(imgUp, &ImgShadowUp);
	lv_obj_align(imgUp, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

	lv_obj_t *imgDown = lv_img_create(contDisp, NULL);
	lv_img_set_src(imgDown, &ImgShadowDown);
	lv_obj_align(imgDown, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

/**
  * @brief  Move to the selected icon
  * @param  iconIndex:index of the target icon
  * @retval None
  */
static void ICON_Grp_MoveFocus(uint8_t iconIndex)
{
	if (iconIndex > ICON_MAX_INDEX)
		return;

	/*Change the title bar text*/
	lv_label_set_static_text(labelTitle, ICON_Grp[iconIndex].text);

	/*Calculate the target Y coordinate*/
	int16_t target_x = -(ICON_Size + ICON_IntervalPixel) * (iconIndex - 1) - ICON_Offset - 10;
	/*Run the slide animation*/
	obj_add_anim(
		contICON,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
		lv_obj_get_x(contICON),					  //Start value
		target_x,								  //End value
		lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
	);
	if (iconIndex == 0)
	{
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			LV_IMG_ZOOM_NONE / 1.5,					  //Start value
			anim_target_zoom,						  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_start_y,							  //Start value
			anim_target_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_zoom,						  //Start value
			LV_IMG_ZOOM_NONE / 1.5,					  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_y,							  //Start value
			anim_start_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
	}
	else if (iconIndex >= 1 && iconIndex < ICON_MAX_INDEX)
	{
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			LV_IMG_ZOOM_NONE / 1.5,					  //Start value
			anim_target_zoom,						  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_start_y,							  //Start value
			anim_target_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_zoom,						  //Start value
			LV_IMG_ZOOM_NONE / 1.5,					  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_y,							  //Start value
			anim_start_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);

		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_zoom,						  //Start value
			LV_IMG_ZOOM_NONE / 1.5,					  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex + 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_y,							  //Start value
			anim_start_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
	}
	else if (iconIndex == ICON_MAX_INDEX)
	{
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_zoom,						  //Start value
			LV_IMG_ZOOM_NONE / 1.5,					  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex - 1].img,			  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_target_y,							  //Start value
			anim_start_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //Animation object
			(lv_anim_exec_xcb_t)lv_img_set_zoom,	  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			LV_IMG_ZOOM_NONE / 1.5,					  //Start value
			anim_target_zoom,						  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
		obj_add_anim(
			ICON_Grp[iconIndex].img,				  //Animation object
			(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
			lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
			anim_start_y,							  //Start value
			anim_target_y,							  //End value
			lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
		);
	}
}

/**
  * @brief  Move the selected icon up and down
  * @param  dir:direction
  * @retval None
  */
static void ICON_Grp_Move(int8_t dir)
{
	/*Move within the allowed range*/
	__ValuePlus(ICON_NowSelIndex, dir, 0, ICON_MAX_INDEX);

	/*Move to the new icon*/
	ICON_Grp_MoveFocus(ICON_NowSelIndex);
}

static void Exit()
{
	/*Slide all icons out*/
	obj_add_anim(
		contICON,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_y,		  //Animation function
		lv_anim_speed_to_time(anim_speed, 0, 50), //Animation speed
		lv_obj_get_y(contICON),					  //Start value
		lv_obj_get_height(contDisp) + ICON_Size,  //End value
		lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	obj_add_anim(
		appWindow,						  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_y, //Animation function
		300,							  //Animation speed
		0,								  //Start value
		APP_WIN_HEIGHT,					  //End value
		lv_anim_path_linear				  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	lv_obj_del(appWindow);
	lv_obj_set_click(lv_layer_top(), false);
	lv_obj_clean(lv_layer_top());
	lv_obj_set_event_cb(lv_layer_top(), NULL); /* Assign the event handler */
}

static void Setup()
{
	//Get the available heap size
	printf("page_menu_start     esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf("page_menu_start     esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	Title_Create();
	Cont_create();
	// ImgShadow_Create();

	obj_add_anim(
		appWindow,						  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_y, //Animation function
		300,							  //Animation speed
		APP_WIN_HEIGHT,					  //Start value
		0,								  //End value
		lv_anim_path_linear				  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	/*Slide to the previously selected icon*/
	ICON_Grp_MoveFocus(ICON_NowSelIndex);
	/* Assign the screen touch event handler */
	lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_page_menu);
}
uint8_t Menu_Choose(void)
{
	return ICON_Grp[ICON_NowSelIndex].pageID;
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
void PageRegister_Menu(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register Menu with the page scheduler */\r\n");
}
