/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-07-17 11:59:30
 * @FilePath: \SP_V2_DEMO\s3_lvgl_v7_xpt2046_0226\main\page\page_cam.c
 */
#include "page_cam.h"
#include "page_start.h"
#include "page_menu.h"
#include "app_main.h"
#include "app_face.h"
#include "EasyTracer.h"
#include "button.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
#include "lv_port_indev.h"
#include "app_camera.h"
#include "fb_gfx.h"
#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#define TAG "PAGE_CAM"

lv_obj_t *img_cam; //Image to display
lv_group_t *group_button;
extern lv_img_dsc_t imgfft;
extern lv_img_dsc_t img_dsc;
extern camera_fb_t *fb;
RESULT Resu;
uint8_t color_type = 0;
TARGET_CONDI Condition[3] = {
	{50, 120, 70, 250, 10, 180, 40, 40, 120, 120},	//Green
	{180, 255, 70, 250, 10, 180, 40, 40, 120, 120}, //Red
	{130, 170, 70, 250, 10, 180, 40, 40, 120, 120}, //Blue
};

void Cam_Task(void *pvParameters)
{

	// /* Detect once at entry */
	ESP_LOGI(TAG, "Run Run uxHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
	// FILE *fp = NULL;
	portTickType xLastWakeTime;

	while (1)
	{
		if (cam_en)
		{
			if (face_en == 0)
			{
				static int64_t last_frame = 0;
				if (!last_frame)
				{
					last_frame = esp_timer_get_time();
				}
				fb = esp_camera_fb_get();
				if (fb == NULL)
				{
					vTaskDelay(100);
					ESP_LOGE(TAG, "Get image failed!");
				}
				else
				{
					if (Trace(&Condition[color_type], &Resu) && color_en == 1)
					{
						printf("x:%d y:%d w:%d h:%d ", Resu.x, Resu.y, Resu.w, Resu.h);
						if (Resu.x > 0 && Resu.y > 0)
						{
							draw_fillRect(fb, Resu.x - Resu.w / 2, Resu.y - Resu.h / 2, Resu.w, Resu.h);
						}
					}
					img_dsc.data = fb->buf;
					lv_img_set_src(img_cam, &img_dsc);

					esp_camera_fb_return(fb);
					fb = NULL;
					int64_t fr_end = esp_timer_get_time();
					int64_t frame_time = fr_end - last_frame;
					last_frame = fr_end;
					frame_time /= 1000;
					ESP_LOGI("esp", "MJPG:  %ums (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
					// vTaskDelayUntil(&xLastWakeTime, (20 / portTICK_RATE_MS));
				}
			}
		}
		else
		{
			if (fb)
			{
				esp_camera_fb_return(fb);
				free(fb);
			}

			fb = NULL;
			vTaskDelete(NULL);
		}
	}

	// never reach
	while (1)
	{
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
}
//What the MOVE task should do when the current screen is the menu
void move_task_cam(uint8_t move)
{

	switch (move)
	{
	case BT1_DOWN: //Move up
		// color_type--;
		// color_type = color_type <= 0 ? 0 : color_type;
		break;
	case BT1_LONG: //Move up

		break;
	case BT1_LONGFREE: //Move up
		break;
	case BT3_DOWN: //Move down
		// color_type++;
		// color_type = color_type >= 2 ? 2 : color_type;
		break;
	case BT3_LONG: //Move down

		break;
	case BT3_LONGFREE: //Move up
		break;

	default:
		break;
	}
}
//What the MOVE task should do when the current screen is the menu
void move_task_color(uint8_t move)
{

	switch (move)
	{
	case BT1_DOWN: //Move up
		color_type--;
		color_type = color_type < 0 ? 0 : color_type;
		break;
	case BT1_LONG: //Move up

		break;
	case BT1_LONGFREE: //Move up
		break;
	case BT3_DOWN: //Move down
		color_type++;
		color_type = color_type > 2 ? 2 : color_type;
		break;
	case BT3_LONG: //Move down

		break;
	case BT3_LONGFREE: //Move up
		break;

	default:
		break;
	}
}
void imgcam_init(void)
{

	img_cam = lv_img_create(lv_scr_act(), NULL);
	static lv_style_t style_img;
	lv_style_init(&style_img);

	//Write style state: LV_STATE_DEFAULT for style_img
	lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
	lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(img_cam, LV_IMG_PART_MAIN, &style_img);
	lv_obj_set_pos(img_cam, 0, 0);
	lv_obj_set_size(img_cam, 240, 240);
}
lv_task_t *task_cam;
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
void page_cam_load()
{
	app_camera_init();
	imgcam_init();
	obj_add_anim(
		img_cam,						   //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		240,							   //Start value
		0,								   //End value
		lv_anim_path_linear				   //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	xTaskCreatePinnedToCore(&Cam_Task, "Cam_Task", 1024 * 5, NULL, 14, NULL, 0);
	lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_touch);
}

static void Exit(void)
{
	cam_en = 0,color_en = 0, face_en = 0;
	vTaskDelay(200);
	esp_camera_deinit();
	obj_add_anim(
		img_cam,						   //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		0,								   //Start value
		240,							   //End value
		lv_anim_path_linear				   //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	if (fb)
	{
		esp_camera_fb_return(fb);
		free(fb);
	}
	lv_obj_del(img_cam);
}
// extern en_fsm_state g_state;

static void Setup_Cam(void)
{
	//Get the available heap size
	printf(" page_cam_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf(" page_cam_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	cam_en = 1;
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_cam_load();
}
static void Setup_Color(void)
{
	//Get the available heap size
	printf(" page_cam_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf(" page_cam_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	cam_en = 1;
	color_en = 1;
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_cam_load();
}
static void Setup_Face(void)
{
	//Get the available heap size
	printf(" page_cam_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf(" page_cam_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());

	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_cam_load();
	face_en = 1;
	g_state = START_RECOGNITION;
	xTaskCreatePinnedToCore(&Face_DEC, "Face_DEC", 1024 * 4, NULL, 5, NULL, 0);
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
void PageRegister_Cam(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup_Cam, NULL, Exit, NULL);
	printf("/* Register Cam with the page scheduler */");
}
/**
  * @brief  Page registration
  * @param  pageID:ID assigned to this page
  * @retval None
  */
void PageRegister_Color(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup_Color, NULL, Exit, NULL);
	printf("/* Register Color with the page scheduler */");
}
/**
  * @brief  Page registration
  * @param  pageID:ID assigned to this page
  * @retval None
  */
void PageRegister_Face(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup_Face, NULL, Exit, NULL);
	printf("/* Register Face with the page scheduler */");
}
