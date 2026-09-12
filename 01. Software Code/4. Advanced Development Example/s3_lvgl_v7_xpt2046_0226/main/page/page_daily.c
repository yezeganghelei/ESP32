/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-13 17:16:36
 * @FilePath     : \esp-idf\pro\KSDIY_ESPCAM\main\page\page_daily.c
 */
#include "page_daily.h"
#include "page_wakeup.h"
#include "base64.h"
#include "urlcode.h"
#include "cJSON.h"
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

#include <esp_system.h>
#include "esp_log.h"
#include "lv_port_indev.h"

#define TAG "DAILY"

#define DAILY_URL "https://api.xygeng.cn/one"
#define HEART_URL "http://www.dutangapp.cn/u/toxic?date=2021-7-13"
/*This page's window*/
static lv_obj_t *appWindow;
/*Title bar*/
static lv_obj_t *labelTitle;

/*Title bar separator line*/
static lv_obj_t *lineTitle;
/*Icon display container, used for clipped display*/
static lv_obj_t *contDisp;

static lv_obj_t *label_sentence;
/**
  * @brief  Create the title bar
  * @param  None
  * @retval None
  */
 extern lv_obj_t *label_speech;
extern const uint8_t example_gif_map[]; /*Use the example gif*/
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
	lv_label_set_static_text(labelTitle, "Daily");
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
	LV_FONT_DECLARE(myFont);
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
	lv_obj_set_size(contDisp, LV_HOR_RES, 190);
	lv_obj_set_pos(contDisp, 0, 60);
	// lv_obj_t *img_gif = lv_gif_create_from_data(contDisp, example_gif_map);
	// lv_obj_set_size(img_gif, 100, 100);
	// lv_obj_set_pos(img_gif, 70, 10);

	label_sentence = lv_label_create(contDisp, NULL);

	static lv_style_t style_label;
	lv_style_init(&style_label);

	//Write style state: LV_STATE_DEFAULT for style_label
	lv_style_set_radius(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, 255);
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_obj_set_style_local_text_font(label_sentence, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
	lv_style_set_text_letter_space(&style_label, LV_STATE_DEFAULT, 2);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_right(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_bottom(&style_label, LV_STATE_DEFAULT, 0);

	lv_obj_add_style(label_sentence, LV_LABEL_PART_MAIN, &style_label);
	lv_label_set_long_mode(label_sentence, LV_LABEL_LONG_BREAK);
	lv_label_set_align(label_sentence, LV_LABEL_ALIGN_CENTER);

	lv_obj_set_size(label_sentence, LV_HOR_RES, LV_VER_RES);
	lv_label_set_recolor(label_sentence, true);

	lv_label_set_static_text(label_sentence, "KS-ESP32-MASTER\nKevincoooool\n"__DATE__);
	lv_obj_align(label_sentence, NULL, LV_ALIGN_CENTER, 0, 0);
	lv_obj_set_auto_realign(label_sentence, true);
}
static esp_err_t daily_http_event_handler(esp_http_client_event_t *evt)
{

	//printf("evtid = %d\n", evt->event_id);
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:

		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
		printf("%.*s\n", evt->data_len, (char *)evt->data);

		cJSON *json_root = cJSON_Parse((char *)evt->data);
		cJSON *result_code = cJSON_GetObjectItem(json_root, "code");

		printf("result_code = %d\n", result_code->valueint);
		if (result_code->valueint != 200)
		{
			cJSON_Delete(json_root);
			return ESP_OK;
		}
		cJSON *result = cJSON_GetObjectItem(json_root, "data");
		cJSON *keyword = cJSON_GetObjectItem(result, "content");
		printf("content = %s\n", keyword->valuestring);

		lv_label_set_text(label_sentence, keyword->valuestring);

		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		page_wakeup_end();
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	}
	return ESP_OK;
}
void get_daily(void)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	lv_label_set_text(label_speech, "Getting daily quote...");
	static esp_http_client_config_t config = {
		.url = DAILY_URL,
		.event_handler = daily_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		page_wakeup_end();
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}
static esp_err_t heart_http_event_handler(esp_http_client_event_t *evt)
{

	//printf("evtid = %d\n", evt->event_id);
	switch (evt->event_id)
	{
	case HTTP_EVENT_ERROR:
		ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:

		printf("HTTP_EVENT_ON_DATA, len=%d\n", evt->data_len);
		printf("%.*s\n", evt->data_len, (char *)evt->data);

		cJSON *json_root = cJSON_Parse((char *)evt->data);

		cJSON *result = cJSON_GetObjectItem(json_root, "data");
		cJSON *result1 = cJSON_GetArrayItem(result, 0);
		cJSON *keyword = cJSON_GetObjectItem(result1, "data");
		printf("content = %s\n", keyword->valuestring);

		lv_label_set_text(label_sentence, keyword->valuestring);
		if (json_root != NULL)
		{
			cJSON_Delete(json_root);
		}
		page_wakeup_end();
		break;

	case HTTP_EVENT_ON_FINISH:
		ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	}
	return ESP_OK;
}
void get_heart(void)
{
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	lv_label_set_text(label_speech, "Getting daily inspiration...");
	static esp_http_client_config_t config = {
		.url = HEART_URL,
		.event_handler = heart_http_event_handler,
		.buffer_size = 3 * 1024,
		.timeout_ms = 4000,
		.path = "/get",
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
	};

	esp_http_client_handle_t client = esp_http_client_init(&config);
	esp_http_client_set_header(client, "Accept", "*/*");
	esp_http_client_set_header(client, "Accept-Encoding", "identity");
	esp_http_client_set_header(client, "User-Agent", "PostmanRuntime/7.24.1");
	esp_http_client_set_header(client, "Connection", "keep-alive");
	esp_http_client_set_header(client, "Content-Type", "application/json");

	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK)
	{
		ESP_LOGE(TAG, "HTTP GET Status = %d, content_length = %d",
				 esp_http_client_get_status_code(client),
				 esp_http_client_get_content_length(client));
	}
	else
	{
		page_wakeup_end();
		ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
	}

	esp_http_client_cleanup(client);
}
//What the MOVE task should do when the current screen is the menu
void move_task_daily(uint8_t move)
{

	switch (move)
	{
	case BT1_DOWN: //Move up
		page_wakeup_start();

		get_heart();
		break;
	case BT1_LONG: //Move up

		break;
	case BT1_LONGFREE: //Move up
		break;
	case BT2_DOWN: //Move down
		page_wakeup_start();

		get_daily();
		break;
	case BT3_LONG: //Move down

		break;
	case BT3_LONGFREE: //Move up
		break;

	default:
		break;
	}
}

void page_daily_load()
{

	Title_Create();
	Cont_create();
	obj_add_anim(
		appWindow,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		APP_WIN_WIDTH,					  //Start value
		0,  //End value
		lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	page_wakeup_start();
	get_daily();
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
static void Exit(void)
{

	obj_add_anim(
		appWindow,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		0,					  //Start value
		APP_WIN_WIDTH,  //End value
		lv_anim_path_linear						  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	lv_obj_del(appWindow);
}

static void Setup(void)
{
	//Get the available heap size
	printf(" page_daily_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf(" page_daily_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	page_daily_load();
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
void PageRegister_Daily(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register Daily with the page scheduler */\r\n");
}
