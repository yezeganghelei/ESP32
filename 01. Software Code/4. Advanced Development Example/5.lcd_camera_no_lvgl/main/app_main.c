/*
 * @Author: Kevincoooool
 * @Date: 2022-01-12 08:57:06
 * @Description:
 * @version:
 * @Filename: Do not Edit
 * @LastEditTime: 2023-07-17 10:16:42
 * @FilePath: \SP_V2_DEMO\5.lcd_camera_no_lvgl\main\app_main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_attr.h"
#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"

// screen
#include "screen.h"

// camera
#include "app_camera.h"
#include "esp_camera.h"

#define CONFIG_MOSI_GPIO 47
#define CONFIG_SCLK_GPIO 21
#define CONFIG_CS_GPIO 14
#define CONFIG_DC_GPIO 45
#define CONFIG_RESET_GPIO -1
#define CONFIG_BL_GPIO 48

#define CONFIG_WIDTH 240
#define CONFIG_HEIGHT 240
#define CONFIG_OFFSETX 0
#define CONFIG_OFFSETY 0

#define INTERVAL 400
#define WAIT vTaskDelay(INTERVAL)

camera_fb_t *fb = NULL;

const int pixel_size = sizeof(uint16_t);
const int screen_width = CONFIG_WIDTH;
const int screen_height = CONFIG_HEIGHT;
const int camera_width = CONFIG_WIDTH;

DRAM_ATTR uint16_t colors[240 * sizeof(uint16_t) * 1];
uint8_t temp = 0;
void CamToScreen(TFT_t *dev, int width, int height)
{
    fb = esp_camera_fb_get();
    static int64_t last_frame = 0;
    last_frame = esp_timer_get_time();
    for (int i = 0; i < fb->len; i += 2)
    {
        temp = fb->buf[i];
        fb->buf[i] = fb->buf[i + 1];
        fb->buf[i + 1] = temp;
    }
    // swap_16
    // int64_t fr_end = esp_timer_get_time();
    // int64_t frame_time = fr_end - last_frame;
    // ESP_LOGI("esp", "Deal :%ums", (uint32_t)frame_time);
    for (int row = 0; row < screen_height;)
    {
        // ESP_LOGI("TAG", "row=%d ", row);
        memcpy(colors, fb->buf + (row * pixel_size * camera_width), pixel_size * screen_width);
        lcdDrawMultiPixels(dev, 0, row, screen_width, colors);
        row += 1;
    }
    esp_camera_fb_return(fb);
    int64_t fr_end = esp_timer_get_time();
    int64_t frame_time = fr_end - last_frame;
    last_frame = fr_end;
    frame_time /= 1000;
    ESP_LOGI("esp", "MJPG:  %ums (%.1ffps)", (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time);
    fb = NULL;
}

void Run(void *pvParameters)
{
    TFT_t dev;
#if CONFIG_XPT2046
    ESP_LOGI(TAG, "Enable XPT2046 Touch Contoller");
    int MISO_GPIO = CONFIG_MISO_GPIO;
    int XPT_CS_GPIO = CONFIG_XPT_CS_GPIO;
    int XPT_IRQ_GPIO = CONFIG_XPT_IRQ_GPIO;
#else
    int MISO_GPIO = -1;
    int XPT_CS_GPIO = -1;
    int XPT_IRQ_GPIO = -1;
#endif

    spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO, 13, 46, 2);
    uint16_t model = 0x7796;
#if CONFIG_ILI9225
    uint16_t model = 0x9225;
#endif
#if CONFIG_ILI9225G
    uint16_t model = 0x9226;
#endif
#if CONFIG_ILI9340
    uint16_t model = 0x9340;
#endif
#if CONFIG_ILI9341
    uint16_t model = 0x9341;
#endif
#if CONFIG_ST7735
    uint16_t model = 0x7735;
#endif
#if CONFIG_ST7796
    uint16_t model = 0x7796;
#endif

    lcdInit(&dev, model, CONFIG_WIDTH, CONFIG_HEIGHT, CONFIG_OFFSETX, CONFIG_OFFSETY);

    while (1)
    {
        CamToScreen(&dev, CONFIG_WIDTH, CONFIG_HEIGHT);
    }

    // never reach
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    /*****************************************************************/
    app_camera_init();//Initialize camera
    xTaskCreate(Run, "Run", 1024 * 6, NULL, 2, NULL);
}