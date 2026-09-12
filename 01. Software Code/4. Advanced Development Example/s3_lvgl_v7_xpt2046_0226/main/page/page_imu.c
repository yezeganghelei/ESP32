
#include "page_imu.h"
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

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"
#include "mpu6050.h"

#define TAG "mpu6050"

/*This page's window*/
static lv_obj_t *appWindow;
/*Title bar*/
static lv_obj_t *labelTitle;

/*Title bar separator line*/
static lv_obj_t *lineTitle;
/*Icon display container, used for clipped display*/
static lv_obj_t *contDisp;

uint8_t imu_en = 0;
static lv_obj_t *chart_imu = NULL;
static lv_chart_series_t *series = NULL;
static lv_chart_series_t *series1 = NULL;

_xyz_f_st Gyro_deg = {0.0, 0.0, 0.0}, Acc_mmss = {0.0, 0.0, 0.0}, Gyro = {0.0, 0.0, 0.0}, Acc = {0.0, 0.0, 0.0};

void Imu_Task(void *pvParameters)
{
	imu_en = 1;
	vTaskDelay(100);
	bsp_i2c_master_init();
	vTaskDelay(100);
	mpu6050_init();
	vTaskDelay(100);
	// /* Detect once at entry */
	ESP_LOGI("Imu_Task", "Imu_Task uxHighWaterMark = %d", uxTaskGetStackHighWaterMark(NULL));
	
	ESP_LOGI(TAG, "connection:%d", mpu6050_test_connection());
	ESP_LOGI(TAG, "tag:%c", *mpu6050_get_tag());
	ESP_LOGI(TAG, "aux_vddio_level:%d", mpu6050_get_aux_vddio_level());
	ESP_LOGI(TAG, "rate divider:%d", mpu6050_get_rate());
	ESP_LOGI(TAG, "external_frame_sync:%d", mpu6050_get_external_frame_sync());

	ESP_LOGI(TAG, "dlpf_mode:%d", mpu6050_get_dlpf_mode());
	ESP_LOGI(TAG, "full_scale_gyro_range:%d", mpu6050_get_full_scale_gyro_range());
	printf("\n mpu6050_test_connection:%d\n", mpu6050_test_connection());
	// Define variables
	mpu6050_acceleration_t accel = {
		.accel_x = 0,
		.accel_y = 0,
		.accel_z = 0,
	};
	mpu6050_rotation_t gyro = {
		.gyro_x = 0,
		.gyro_y = 0,
		.gyro_z = 0,
	};

	while (1)
	{
		if (imu_en)
		{

			mpu6050_get_motion(&accel, &gyro);
			//printf("ax:%d;ay:%d;az:%d", accel.accel_x, accel.accel_y, accel.accel_z);
			Gyro_deg.x = gyro.gyro_x * 0.0610361f; //  /65535 * 4000; +-2000 degrees
			Gyro_deg.y = gyro.gyro_y * 0.0610361f;
			Gyro_deg.z = gyro.gyro_z * 0.0610361f;

			/*Convert accelerometer readings to millimeters per second squared*/
			Acc_mmss.x = accel.accel_x * 2.392615f; //   /65535 * 16*9800; +-8G
			Acc_mmss.y = accel.accel_y * 2.392615f;
			Acc_mmss.z = accel.accel_z * 2.392615f;
			// printf("ax:%7.2f;ay:%7.2f;az:%7.2f", Acc_mmss.x, Acc_mmss.y, Acc_mmss.z);
			// printf("gx:%7.2f;gy:%7.2f;gz:%7.2f", Gyro_deg.x, Gyro_deg.y, Gyro_deg.z);
			IMU_update(0.005f, &Gyro_deg, &Acc_mmss, &imu_data); //Attitude estimation

			// printf("pitch:%3.2f, yaw:%3.2f, roll:%3.2f\n", imu_data.pit, imu_data.yaw, imu_data.rol);
			lv_chart_set_next(chart_imu, series, imu_data.pit);
			lv_chart_set_next(chart_imu, series1, imu_data.rol);
			// vTaskDelayUntil(&xLastWakeTime, (2 / portTICK_RATE_MS));
			vTaskDelay(5);
		}
		else
		{
			bsp_i2c_master_deinit();
			vTaskDelete(NULL);
		}
	}
}
/**
  * @brief  Create the title bar
  * @param  None
  * @retval None
  */
static void Title_Create()
{
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
	lv_obj_set_pos(appWindow, 0, 0);
	lv_obj_set_size(appWindow, APP_WIN_WIDTH, APP_WIN_HEIGHT);

	labelTitle = lv_label_create(appWindow, NULL);

	static lv_style_t style_label;
	lv_style_init(&style_label);

	//Write style state: LV_STATE_DEFAULT for style_label
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
	lv_obj_set_size(labelTitle, APP_WIN_WIDTH, 55);
	lv_label_set_recolor(labelTitle, true);

	/*The second icon is selected by default*/
	lv_label_set_static_text(labelTitle, "IMU");
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
	lv_obj_set_size(contDisp, APP_WIN_WIDTH, APP_WIN_HEIGHT - 60);
	lv_obj_set_pos(contDisp, 0, 60);

	chart_imu = lv_chart_create(contDisp, NULL);
	lv_obj_set_size(chart_imu, APP_WIN_WIDTH, APP_WIN_HEIGHT - 60);
	lv_chart_set_point_count(chart_imu, 50);
	lv_obj_align(chart_imu, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_chart_set_type(chart_imu, 5);
	static lv_style_t style_screen_chart3_series_bg;
	lv_style_init(&style_screen_chart3_series_bg);

	//Write style state: LV_STATE_DEFAULT for style_screen_chart3_series_bg
	lv_style_set_line_color(&style_screen_chart3_series_bg, LV_STATE_DEFAULT, lv_color_make(0xe8, 0xe8, 0xe8));
	lv_style_set_line_width(&style_screen_chart3_series_bg, LV_STATE_DEFAULT, 2);
	lv_style_set_line_opa(&style_screen_chart3_series_bg, LV_STATE_DEFAULT, 255);
	lv_obj_add_style(chart_imu, LV_CHART_PART_SERIES_BG, &style_screen_chart3_series_bg);
	lv_chart_set_type(chart_imu, LV_CHART_TYPE_LINE);
	lv_chart_set_range(chart_imu, -(APP_WIN_HEIGHT - 60), APP_WIN_HEIGHT - 60);
	lv_chart_set_y_tick_length(chart_imu, 10, 5);

	lv_obj_set_style_local_bg_opa(chart_imu, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0); /*Max. opa.*/
	lv_obj_set_style_local_bg_grad_dir(chart_imu, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_obj_set_style_local_bg_main_stop(chart_imu, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0); /*Max opa on the top*/
	lv_obj_set_style_local_bg_grad_stop(chart_imu, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 0); /*Transparent on the bottom*/

	//6.13 Set the y-axis major tick labels and the number of ticks between them

	lv_chart_set_y_tick_texts(chart_imu, "180\n150\n120\n90\n60\n30\n0", 7, LV_CHART_AXIS_DRAW_LAST_TICK);
	series = lv_chart_add_series(chart_imu, LV_COLOR_RED);
	series1 = lv_chart_add_series(chart_imu, LV_COLOR_BLUE);
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
void page_imu_load()
{
	imu_en = 1;
	Title_Create();
	Cont_create();
	obj_add_anim(
		appWindow,						   //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		APP_WIN_WIDTH,					   //Start value
		0,								   //End value
		lv_anim_path_linear				   //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	xTaskCreatePinnedToCore(&Imu_Task, "Imu_Task", 1024 * 3, NULL, 17, NULL, 0);
	
}
static void Exit(void)
{
	imu_en = 0;
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
	printf(" page_imu_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf(" page_imu_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_imu_load();
		lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_touch);
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
void PageRegister_IMU(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register IMU with the page scheduler */\r\n");
}
