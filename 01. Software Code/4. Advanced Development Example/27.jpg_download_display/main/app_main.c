/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-09-04 16:11:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-07-17 17:09:37
 * @FilePath: \SP_V2_DEMO\27.jpg_download_display\main\app_main.c
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include "lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
#include "lvgl_helpers.h"
#include "esp_freertos_hooks.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "app_main.h"
#include "app_wifi.h"
#include "lv_png.h"
#include "lv_sjpg.h"
#include "bili_http.h"

#define TAG "ESP32S3"
/*Give with timerLVGLProvide clock*/
#include "driver/gpio.h"
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}

SemaphoreHandle_t xGuiSemaphore;
lv_img_dsc_t bili_face = {
    .header.always_zero = 0,
    .header.w = 300,
    .header.h = 300,
    .data_size = 300 * 300 * 2,
    .header.cf = LV_IMG_CF_RAW,
    .data = NULL,
};
lv_obj_t *img_face;

LV_IMG_DECLARE(wallpaper_jpg);
LV_IMG_DECLARE(small_image_sjpg);
static void gui_task(void *arg)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init(); // lvgl kernel initialization

    lvgl_driver_init(); // lvgl显示接口initialization
    /*externalPSRAMHow to applybufferfor screen refresh*/
    // lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    /*internalDMAWay*/
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

    /*static array mode*/
    // static lv_color_t buf1[DISP_BUF_SIZE];
    // static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);
    /*Display driver interface configuration refresh function*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    /*Touch screen input interface configuration*/
    // lv_indev_drv_t indev_drv;
    // lv_indev_drv_init(&indev_drv);
    // indev_drv.read_cb = touch_driver_read;
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // lv_indev_drv_register(&indev_drv);

    // esp_register_freertos_tick_hook(lv_tick_task);
    /* Create a timer interrupt to enter lv_tick_inc to provide a heartbeat for lvgl running. Here it is every 10ms. It is mainly used for animation running. */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));
    lv_png_init();
    lv_split_jpeg_init();
    img_face = lv_img_create(lv_scr_act(), NULL);

    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));
        bili_face.data = (uint8_t *)face_buffer; //Give the buffer obtained online to img

        lv_img_set_src(img_face, &bili_face); //
        // lv_img_set_src(img_face, &wallpaper_jpg); //

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {

            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}

/*
    When ESP32 works, app_main is used as the main entrance, and STM32 is main.
*/
void app_main(void)
{

    // initializationnvsfor storagewifiOr other things that need to be saved after power off
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    app_wifi_init();
    get_face_jpg("http://i2.hdslb.com/bfs/face/bce14f5e3af4bca480fc7de227986ba304507078.jpg");
    // get_face_jpg("https://aithinker-static.oss-cn-shenzhen.aliyuncs.com/officialwebsite/banner/BLE-Mesh.png");

    /*Create lvgl task display*/
    xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 1);
}