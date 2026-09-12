/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-09-04 16:11:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-04-28 19:51:26
 * @FilePath: \S3_DEMO\17.avi_recorder\main\app_main.c
 */
#include "app_main.h"
#include "lv_examples/src/lv_demo_widgets/lv_demo_widgets.h"
#include "lv_examples/src/lv_demo_music/lv_demo_music.h"
#include "lv_examples/src/lv_demo_benchmark/lv_demo_benchmark.h"
#include "lvgl_helpers.h"
#include "esp_freertos_hooks.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "avi_recorder.h"
#include "avi_player.h"
#include "app_camera.h"
#include "page_cam.h"
#include "file_manager.h"
#define TAG "ESP32S3"
/*Use a timer to provide LVGL with its clock tick*/
#include "driver/gpio.h"
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}

SemaphoreHandle_t xGuiSemaphore;

static void gui_task(void *arg)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init(); // lvgl kernel initialization

    lvgl_driver_init(); // lvgl display interface initialization
    // Apply for two buffers for lvgl to refresh the screen
    /*External PSRAM mode*/
    // lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    /*Internal DMA mode*/
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);

    /*static array mode*/
    // static lv_color_t buf1[DISP_BUF_SIZE];
    // static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

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

    // app_camera_init();//Initialize the camera
    //avi_recorder_start("/sdcard/record.avi", FRAMESIZE_VGA, 10*3,1);//record a30Seconds of video to/sdcard/record.avi
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {

            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}
// ffmpeg -i original video.mp4 -vcodec mjpeg -vf scale=240:240 -r 14 -acodec pcm_s16le -ar 32000 tom-240.avi
#define EXAMPLE_SENSOR_FRAME_SIZE FRAMESIZE_VGA

static uint32_t camera_test_fps(uint16_t times)
{
    uint32_t image_size = 0;
    uint32_t ret;
    ESP_LOGI(TAG, "satrt to test fps");
    esp_camera_fb_return(esp_camera_fb_get());
    esp_camera_fb_return(esp_camera_fb_get());

    uint64_t total_time = esp_timer_get_time();
    for (size_t i = 0; i < times; i++) {
        camera_fb_t *pic = esp_camera_fb_get();
        if (NULL == pic) {
            ESP_LOGW(TAG, "fb get failed");
            continue;
        }

        image_size += pic->len;
        esp_camera_fb_return(pic);
    }
    total_time = esp_timer_get_time() - total_time;
    float fps = times / (total_time / 1000000.0f);
    ret = image_size / times;
    ESP_LOGI(TAG, "fps=%f, image_average_size=%u", fps, ret);
    return ret;
}

static int _get_frame(void **buf, size_t *len)
{
    camera_fb_t *image_fb = esp_camera_fb_get();
    if (!image_fb) {
        ESP_LOGE(TAG, "Camera capture failed");
        return -1;
    } else {
        ESP_LOGI(TAG, "len=%d", image_fb->len);
        *buf = &image_fb->buf;
        *len = image_fb->len;
    }
    return 0;
}

static int _return_frame(void *inbuf)
{
    camera_fb_t *image_fb = __containerof(inbuf, camera_fb_t, buf);
    esp_camera_fb_return(image_fb);
    return 0;
}

void app_main(void)
{
    const char *video_file = "/sdcard/recorde.avi";

    // Initialize NVS for storing WiFi or other data that must persist across power-off
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    fm_sdcard_init();
    /*Create lvgl task display*/
    // xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 1);
    app_camera_init();                                                  // Initialize camera

    // avi_recorder_start("/sdcard/record.avi", FRAMESIZE_QVGA, 10 * 3, 0); // record a30Seconds of video to/sdcard/record.avi
     avi_recorder_start(video_file, _get_frame, _return_frame, resolution[EXAMPLE_SENSOR_FRAME_SIZE].width, resolution[EXAMPLE_SENSOR_FRAME_SIZE].height, 30 * 1, 1);

}