/*
 * @Descripttion :  
 * @version      :  
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2021-11-13 17:16:11
 * @FilePath     : \s3_lvgl_v7\main\page\page_fft.c
 */
#include "page_fft.h"
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
#include "math.h"
#include "freertos/queue.h"
#include "fft.h"
#include "lvgl/lvgl.h"
#include "driver/i2s.h"


/*This page's window*/
static lv_obj_t *appWindow;
/*Title bar*/
static lv_obj_t *labelTitle;

/*Title bar separator line*/
static lv_obj_t *lineTitle;
/*Icon display container, used for clipped display*/
static lv_obj_t *contDisp;

extern uint8_t fft_en;

lv_obj_t *chart_fft;
lv_chart_series_t *series_fft;

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
	lv_obj_set_size(labelTitle, APP_WIN_WIDTH, 60);
	lv_label_set_recolor(labelTitle, true);

	/*The second icon is selected by default*/
	lv_label_set_static_text(labelTitle, "Spectrum");
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
	lv_style_set_bg_opa(&style_cont, LV_STATE_DEFAULT, 50);
	lv_style_set_pad_top(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_width(&style_cont, LV_STATE_DEFAULT, 0);
	lv_style_set_border_opa(&style_cont, LV_STATE_DEFAULT, 50);
	lv_style_set_bg_color(&style_cont, LV_STATE_DEFAULT, LV_COLOR_BLACK); //Set the screen background
	lv_obj_add_style(contDisp, LV_BTN_PART_MAIN, &style_cont);			  /*Default button style*/
	lv_obj_set_size(contDisp, APP_WIN_WIDTH, APP_WIN_HEIGHT-60);
	lv_obj_set_pos(contDisp, 0, 60);

	chart_fft = lv_chart_create(contDisp, NULL);
	lv_obj_set_size(chart_fft, APP_WIN_WIDTH, APP_WIN_HEIGHT-60);
	lv_chart_set_point_count(chart_fft, 50);
	lv_obj_align(chart_fft, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_local_bg_opa(chart_fft, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 100); /*Max. opa.*/
    lv_obj_set_style_local_bg_grad_dir(chart_fft, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_obj_set_style_local_bg_main_stop(chart_fft, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);    /*Max opa on the top*/
    lv_obj_set_style_local_bg_grad_stop(chart_fft, LV_CHART_PART_SERIES, LV_STATE_DEFAULT, 255);      /*Transparent on the bottom*/


	lv_chart_set_type(chart_fft, LV_CHART_TYPE_COLUMN);
	lv_chart_set_range(chart_fft, 0, APP_WIN_HEIGHT);

	series_fft = lv_chart_add_series(chart_fft, LV_COLOR_RED);



}
//What the MOVE task should do when the current screen is the menu
void move_task_fft(uint8_t move)
{

    switch (move)
    {
    case BT1_DOWN: //Move up
        lv_chart_set_type(chart_fft, LV_CHART_TYPE_LINE);
        break;
    case BT1_LONG: //Move up

        break;
    case BT1_LONGFREE: //Move up
        break;
    case BT3_DOWN: //Move down
        lv_chart_set_type(chart_fft, LV_CHART_TYPE_COLUMN);
        break;
    case BT3_LONG: //Move down

        break;
    case BT3_LONGFREE: //Move up
        break;

    default:
        break;
    }
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
	fft_en = 0;
	obj_add_anim(
		appWindow,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		0,					  //Start value
		APP_WIN_WIDTH,  //End value
		lv_anim_path_ease_out						  //Animation effect: simulate a bouncing object falling
	);
	
	ANIEND
	lv_obj_del(appWindow);
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
}

static void Setup(void)
{
	
	//Get the available heap size
	printf(" page_fft_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	//Get the minimum free heap size ever
	printf(" page_fft_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	Title_Create();
	Cont_create();
	obj_add_anim(
		appWindow,								  //Animation object
		(lv_anim_exec_xcb_t)lv_obj_set_x,		  //Animation function
		lv_anim_speed_to_time(300, 0, 50), //Animation speed
		APP_WIN_WIDTH,					  //Start value
		0,  //End value
		lv_anim_path_ease_out						  //Animation effect: simulate a bouncing object falling
	);
	ANIEND
	fft_en = 1;
	
	xTaskCreatePinnedToCore(&FFT_Task, "FFT_Task", 1024 * 8, NULL, 6, NULL, 0);
		lv_obj_set_click(lv_layer_top(), true);
	lv_obj_set_event_cb(lv_layer_top(), event_handler_touch);
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	long divisor = (in_max - in_min);
	if (divisor == 0)
	{
		return -1; //AVR returns -1, SAM returns 0
	}
	return (x - in_min) * (out_max - out_min) / divisor + out_min;
}

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 240

uint8_t fft_en = 0;
uint8_t fft_dis_buff[240] = {0};
#define SAMPLES_NUM (512)
extern lv_obj_t *chart_fft;
extern lv_chart_series_t *series_fft;
#define IIS_SCLK 16
#define IIS_LCLK 7
#define IIS_DSIN -1
#define IIS_DOUT 15
static void i2s_init(void)
{
	i2s_config_t i2s_config = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
		.sample_rate = 16000,
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		.communication_format = I2S_COMM_FORMAT_STAND_I2S,

		.intr_alloc_flags = ESP_INTR_FLAG_LOWMED,
		.dma_buf_count = 2,
		.dma_buf_len = SAMPLES_NUM,
	};
	i2s_pin_config_t pin_config = {
		.bck_io_num = IIS_SCLK,	  // IIS_SCLK
		.ws_io_num = IIS_LCLK,	  // IIS_LCLK
		.data_out_num = IIS_DSIN, // IIS_DSIN
		.data_in_num = IIS_DOUT	  // IIS_DOUT
	};
	i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
	i2s_set_pin(I2S_NUM_0, &pin_config);
	i2s_zero_dma_buffer(I2S_NUM_0);
}


/**
 * @descripttion: FFT spectrum task
 * @param {void} *arg
 * @return {*}
 */
void FFT_Task(void *arg)
{
	static int8_t i2s_readraw_buff[1024];
	size_t bytesread;
	int16_t *buffptr;
	int16_t *samples_sc16 = (int16_t *)i2s_readraw_buff;
	float *samples_fc32 = (float *)calloc(SAMPLES_NUM, sizeof(float));
	double data = 0;
	// uint8_t *fft_dis_buff = NULL;
	i2s_init();
	
	while (1)
	{
		if (fft_en == 1)
		{

			i2s_read(I2S_NUM_0, (char *)i2s_readraw_buff, SAMPLES_NUM * 2, &bytesread, (100 / portTICK_RATE_MS));
			fft_config_t *real_fft_plan = fft_init(512, FFT_REAL, FFT_FORWARD, NULL, NULL);
			buffptr = (int16_t *)i2s_readraw_buff;
			for (uint16_t count_n = 0; count_n < real_fft_plan->size; count_n++)
			{
				real_fft_plan->input[count_n] = (float)map(buffptr[count_n], INT16_MIN, INT16_MAX, -1000, 1000);
			}
			fft_execute(real_fft_plan);
			for (uint16_t count_n = 1; count_n < CANVAS_HEIGHT; count_n++)
			{
				data = sqrt(real_fft_plan->output[2 * count_n] * real_fft_plan->output[2 * count_n] + real_fft_plan->output[2 * count_n + 1] * real_fft_plan->output[2 * count_n + 1]);
				fft_dis_buff[CANVAS_HEIGHT - count_n] = map(data, 0, 2000, 0, 240);
			}
			fft_destroy(real_fft_plan);
			
			for (uint16_t count_y = 0; count_y < 240;)
			{
				lv_chart_set_next(chart_fft, series_fft, fft_dis_buff[count_y]);
				count_y += 5;
			}
		}
		else
		{
			i2s_driver_uninstall(I2S_NUM_0);
			vTaskDelete(NULL);
		}
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
void PageRegister_FFT(uint8_t pageID)
{
	/*Get the window assigned to this page*/
	// appWindow = AppWindow_GetCont(pageID);

	/*Register with the page scheduler*/
	page.PageRegister(pageID, Setup, NULL, Exit, NULL);
	printf("/* Register FFT with the page scheduler */\r\n");
}
