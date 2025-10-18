/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-06-05 10:13:51
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-07-17 15:28:17
 * @FilePath: \SP_V2_DEMO\11.spectrum\main\page\page_fft.c
 */
#include "page_fft.h"
#include "app_main.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lvgl/lvgl.h"
#include "lvgl_helpers.h"
// #include "lv_port_indev.h"
#include <esp_system.h>
#include "esp_log.h"
#include "math.h"
// #include "number.h"
#include "fft.h"

#include "driver/i2s.h"
#include "spectrum.h"
#include "esp_dsp.h"
#define APP_WIN_HEIGHT 240
#define APP_WIN_WIDTH 240

#define DISP_MAX_HOR LV_HOR_RES
#define DISP_MAX_VER LV_VER_RES

#define ANIEND                      \
	while (lv_anim_count_running()) \
		lv_task_handler(); // Wait for animation to complete

/*This page window*/
static lv_obj_t *appWindow;
/*title bar*/
static lv_obj_t *labelTitle;

/*Title bar divider*/
static lv_obj_t *lineTitle;
/*Icon display container，for cropped display*/
static lv_obj_t *contDisp;

extern uint8_t fft_en;

lv_obj_t *chart_fft;
lv_chart_series_t *series_fft;

/**
 * @brief Create title bar
 * @param none
 * @retval None
 */
static void Title_Create()
{
	LV_FONT_DECLARE(number);

}
// Create a containing box interface
static void Cont_create(void)
{

}

static void Exit(void)
{
	fft_en = 0;
	// obj_add_anim(
	// 	appWindow,						   //animation object
	// 	(lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
	// 	lv_anim_speed_to_time(300, 0, 50), //animation speed
	// 	0,								   //starting value
	// 	APP_WIN_WIDTH, //End value
	// 	lv_anim_path_ease_out			   //animation effects:Simulate the fall of elastic objects
	// );

	// ANIEND
	lv_obj_del(appWindow);
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
}

void FFT_Setup(void)
{

	// Get the available memory of the chip
	printf(" page_fft_start    esp_get_free_heap_size : %d  \n", esp_get_free_heap_size());
	// Get the smallest amount of memory that has never been used
	printf(" page_fft_start    esp_get_minimum_free_heap_size : %d  \n", esp_get_minimum_free_heap_size());
	printf("%s !Dram: %d bytes\r\n", __func__, heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
	Title_Create();
	Cont_create();
	// obj_add_anim(
	// appWindow, // animation object
	// (lv_anim_exec_xcb_t)lv_obj_set_x, //Animation function
	//lv_anim_speed_to_time(300, 0, 50), //Animation speed
	// APP_WIN_WIDTH, //Start value
	// 0, //End value
	//lv_anim_path_ease_out //Animation special effects: simulate the fall of elastic objects
	// );
	//ANIEND
	fft_en = 1;

	xTaskCreatePinnedToCore(&FFT_Task, "FFT_Task", 1024 * 8, NULL, 6, NULL, 0);
	// lv_obj_set_click(lv_layer_top(), true);
	// lv_obj_set_event_cb(lv_layer_top(), event_handler_touch);
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	long divisor = (in_max - in_min);
	if (divisor == 0)
	{
		return -1; // AVR returns -1, SAM returns 0
	}
	return (x - in_min) * (out_max - out_min) / divisor + out_min;
}

#define CANVAS_WIDTH 240
#define CANVAS_HEIGHT 240

uint8_t fft_en = 0;
uint8_t fft_dis_buff[512] = {0};
#define SAMPLES_NUM (512)
extern lv_obj_t *chart_fft;
extern lv_chart_series_t *series_fft;
#define IIS_SCLK 41
#define IIS_LCLK 42
#define IIS_DSIN 2
#define IIS_DOUT 2
void i2s_init(void)
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
// __attribute__((aligned(16))) float wind[SAMPLES_NUM];
/**
 * @descripttion: fftspectrum mission
 * @param {void} *arg
 * @return {*}
 */
void FFT_Task(void *arg)
{
	int32_t *i2s_buff=(int32_t *)calloc(SAMPLES_NUM*2, sizeof(int32_t));

	size_t bytesread;
	int16_t *buffptr;

	double data = 0;
	i2s_init();
	esp_err_t ret;

	while (1)
	{

        i2s_read(I2S_NUM_0, i2s_buff, SAMPLES_NUM * 2, &bytesread, portMAX_DELAY);

		fft_config_t *real_fft_plan = fft_init(SAMPLES_NUM, FFT_REAL, FFT_FORWARD, NULL, NULL);
		buffptr = (int16_t *)i2s_buff;
		for (uint16_t count_n = 0; count_n < real_fft_plan->size; count_n++)
		{
			real_fft_plan->input[count_n] = (float)map(buffptr[count_n], INT16_MIN, INT16_MAX, -1000, 1000);
		}
		fft_execute(real_fft_plan);
		for (uint16_t count_n = 1; count_n < 256; count_n++)
		{
			data = sqrt(real_fft_plan->output[2 * count_n] * real_fft_plan->output[2 * count_n] + real_fft_plan->output[2 * count_n + 1] * real_fft_plan->output[2 * count_n + 1]);
			fft_dis_buff[256 - count_n] = map(data, 0, 2000, 0, 240);
		}
		fft_destroy(real_fft_plan);

		generate_spectrum(fft_dis_buff, 256);

	}
}
/**
 * @brief page event
 * @param btn: the key that emits the event
 * @param event: event number
 * @retval None
 */
static void Event(void *btn, int event)
{
}