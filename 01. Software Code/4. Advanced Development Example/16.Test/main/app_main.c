/*
 * @Descripttion :
 * @version      :
 * @Author       : Kevincoooool
 * @Date         : 2021-09-04 16:11:59
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-08-10 17:02:43
 * @FilePath: \SP_V2_DEMO\16.Test\main\app_main.c
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
#include "page_imu.h"
#include "button.h"
#include "bsp_adc.h"
#include "mpu6050.h"
#include "app_camera.h"
#include "app_led.h"
#include "app_speech_srcif.h"
#include "file_manager.h"
#define TAG "ESP32S3"

lv_obj_t *cont_time;
uint8_t lvgl_init_finish = 0;
/*Give with timerLVGLProvide clock*/
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}
void button_task(void *arg)
{
    Button_Init();
    while (1)
    {
        Button_Process();
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
camera_fb_t *fb;
lv_obj_t *img_cam; // 要show图像
lv_img_dsc_t img_dsc = {
    .header.always_zero = 0,
    .header.w = 96,
    .header.h = 96,
    .data_size = 96 * 96 * 2,
    .header.cf = LV_IMG_CF_TRUE_COLOR,
    .data = NULL,
};

lv_obj_t *label_test;
lv_obj_t *label_btn, *label_adc;
void label_init(void)
{
    cont_time = lv_cont_create(lv_scr_act(), NULL);
    lv_cont_set_fit2(cont_time, LV_FIT_NONE, LV_FIT_NONE);
    lv_obj_set_size(cont_time, 240, 240);
    lv_obj_set_pos(cont_time, 0, 0);
    label_test = lv_label_create(cont_time, NULL);
    /*Modify the Label's text*/

    img_cam = lv_img_create(lv_scr_act(), NULL);
    static lv_style_t style_img;
    lv_style_init(&style_img);

    // Write style state: LV_STATE_DEFAULT for style_img
    lv_style_set_image_recolor(&style_img, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_img, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_img, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(img_cam, LV_IMG_PART_MAIN, &style_img);
    lv_obj_set_pos(img_cam, 140, 10);
    lv_obj_set_size(img_cam, 96, 96);

    static lv_style_t style_label_test;
    lv_style_init(&style_label_test);

    // Write style state: LV_STATE_DEFAULT for style_label_test
    lv_style_set_radius(&style_label_test, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_label_test, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_grad_color(&style_label_test, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_style_set_bg_grad_dir(&style_label_test, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_label_test, LV_STATE_DEFAULT, 255);
    lv_style_set_text_color(&style_label_test, LV_STATE_DEFAULT, LV_COLOR_BLACK);

    // lv_obj_set_style_local_text_font(label_test, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &myFont);
    lv_style_set_text_letter_space(&style_label_test, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_label_test, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_label_test, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_label_test, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_label_test, LV_STATE_DEFAULT, 0);

    lv_obj_add_style(label_test, LV_LABEL_PART_MAIN, &style_label_test);
    lv_label_set_long_mode(label_test, LV_LABEL_LONG_BREAK);
    lv_obj_set_pos(label_test, 0, 0);
    lv_obj_set_size(label_test, LV_HOR_RES, LV_VER_RES);
    lv_label_set_recolor(label_test, true);
    lv_label_set_text(label_test, "Hello World!\n");
}

static void event_handler(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        printf("Clicked\n");
    }
    else if (event == LV_EVENT_VALUE_CHANGED)
    {
        printf("Toggled\n");
    }
}
/*create一个按钮*/
void lv_ex_btn_1(void)
{
    lv_obj_t *btn1 = lv_btn_create(cont_time, NULL);
    lv_obj_set_event_cb(btn1, event_handler);

    lv_obj_set_size(btn1, 180, 30);
    lv_obj_align(btn1, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

    label_btn = lv_label_create(btn1, NULL);
    // lv_obj_set_size(label_btn, 180, 30);
    lv_label_set_text(label_btn, "Button");

    label_adc = lv_label_create(cont_time, NULL);
    lv_obj_set_size(label_adc, 200, 30);
    // lv_obj_align(label_adc, btn1, LV_ALIGN_OUT_TOP_LEFT, 0, -20);
    lv_obj_align(label_adc, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -60);

    lv_label_set_text(label_adc, "ADC:");
}

SemaphoreHandle_t xGuiSemaphore;

static void gui_task(void *arg)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init(); // lvgl kernel initialization

    lvgl_driver_init(); // lvglshow接口initialization
    // Apply for two buffers for lvgl to refresh the screen
    /*externalPSRAMWay*/
    // lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    /*internalDMAWay*/
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * 2 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * 2 * sizeof(lv_color_t), MALLOC_CAP_DMA);

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

    label_init();
    lv_ex_btn_1();
    // lv_demo_widgets();
    // lv_demo_music();
    // lv_demo_benchmark();
    // page_imu_load();
    lvgl_init_finish = 1;
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
/*showspiffsAll file names of*/
static void SPIFFS_Directory(char *path)
{
    DIR *dir = opendir(path);
    assert(dir != NULL);
    while (true)
    {
        struct dirent *pe = readdir(dir);
        if (!pe)
            break;
        ESP_LOGI(__FUNCTION__, "d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
    }
    closedir(dir);
}
bool cam_init_ok = true;
char init_str[4096] = {0};

void app_main(void)
{
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
    /*initializationspiffsfor storage字体文件或者图片文件或者网页文件*/
    ESP_LOGI(TAG, "Initializing SPIFFS");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 20,
        .format_if_mount_failed = false};
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        else if (ret == ESP_ERR_NOT_FOUND)
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        else
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        return;
    }
    /*showspiffsfile list in*/
    SPIFFS_Directory("/spiffs/");

    // initializationnvsfor storagewifiOr other things that need to be saved after power off
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /*Create lvgl task display*/
    xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 8, NULL, 1);

    while (1)
    {
        if (lvgl_init_finish)
        {
            lvgl_init_finish = 0;
            bsp_i2c_master_init();
            vTaskDelay(100);
            mpu6050_init();
            vTaskDelay(100);
            strcpy(init_str, "KS DIY\nHardware Test\n");

            vTaskDelay(100);
            // bsp_i2c_master_deinit();
            /*initializationWS2812 */
            app_led_init(GPIO_RMT_LED);
            vTaskDelay(500);
            app_led_set_all(255, 0, 0);
            vTaskDelay(500);
            app_led_set_all(0, 255, 0);
            vTaskDelay(500);
            app_led_set_all(0, 0, 255);
            vTaskDelay(500);
            app_led_set_all(5, 5, 5);
            lv_label_set_text(label_test, strcat(init_str, "#0000ff WS2812 ok...#\n"));
            if (mpu6050_test_connection())
            {
                strcat(init_str, "#00ff00 MPU6050 ok....#\n");

                mpu6050_get_motion(&accel, &gyro);
                printf("ax:%d;ay:%d;az:%d", accel.accel_x, accel.accel_y, accel.accel_z);
                char *buff = malloc(255);
                sprintf(buff, "ax:%d;ay:%d;az:%d\n", accel.accel_x, accel.accel_y, accel.accel_z);
                strcat(init_str, buff);
                lv_label_set_text(label_test, init_str);
            }
            else
            {
                strcat(init_str, "#ff0000 MPU6050 failed....#\n");
                lv_label_set_text(label_test, init_str);
            }
            if (app_camera_init() == ESP_OK)
            {
                cam_init_ok = true;
                vTaskDelay(200);
                lv_label_set_text(label_test, strcat(init_str, "#00ff00 Camera ok...#\n"));
                fb = esp_camera_fb_get();
                img_dsc.data = fb->buf;
                lv_img_set_src(img_cam, &img_dsc);

                esp_camera_fb_return(fb);
            }
            else
            {
                cam_init_ok = false;
                vTaskDelay(200);
                lv_label_set_text(label_test, strcat(init_str, "#ff0000 Camera failed...#\n"));
                // esp_camera_deinit();
                bsp_i2c_master_init();
            }
            app_led_set_all(0, 0, 0); // Turn off the lights
            break;
        }
    }
    adc_init();
    printf("adc_value: %d\n", get_adc());
    /*create按键任务 Scan key values ​​regularly*/
    xTaskCreatePinnedToCore(&button_task, "button_task", 1024 * 3, NULL, 8, NULL, 0);
    lv_label_set_text(label_test, strcat(init_str, "Speech Init ok...\nPlease say 'Hi Lexin'\n"));
    /*Initialize voice wake-up recognition task*/
    app_speech_wakeup_init();
    //  bsp_i2c_master_init();
    while (1)
    {
        mpu6050_get_motion(&accel, &gyro);

        // printf("adc_value: %d\n", get_adc());
        lv_label_set_text_fmt(label_adc, "ax:%5d;ay:%5d;az:%5d\nadc:%d", accel.accel_x, accel.accel_y, accel.accel_z, get_adc());

        if (Button_Value != BT_NONE) // If the button status changes
        {
            // lv_label_set_text(label_test, strcat(init_str, Button_Tips[Button_Value]));
            lv_label_set_text(label_btn, Button_Tips[Button_Value]);

            Button_Value = BT_NONE;
        }

        if (cam_init_ok != false) // If the camera initialization is successful
        {
            fb = esp_camera_fb_get();          // Get camera pictures
            img_dsc.data = fb->buf;            // Assign the data of the camera image
            lv_img_set_src(img_cam, &img_dsc); // show

            esp_camera_fb_return(fb); // Return the image. Return the image every time you get it.
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}