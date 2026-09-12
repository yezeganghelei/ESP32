/**
 ****************************************************************************************************
 * @file        lvgl_demo.c
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-12-01
 * @brief       LVGL V8 porting example
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "lvgl_demo.h"
#include "demos/lv_demos.h"
#include "app_ui.h"
#include "image.h"
#include "sdmmc_cmd.h"


lv_indev_t *indev_keypad;       /* Button group */
uint32_t back_act_key = 0;      /* Return-to-main-screen button */
uint8_t sd_check_en = 0;        /* SD card detection flag */
uint8_t decode_en = ESP_OK;     /* Can exit only after image decoding */
SemaphoreHandle_t xGuiSemaphore;

/* Function declarations */
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
static void keypad_init(void);
static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
static uint32_t keypad_get_key(void);
void lv_port_indev_init(void);
void increase_lvgl_tick(void *arg);
void lv_port_disp_init(void);


/* WATCH task configuration
 * Includes: task handle, task priority, stack size, task creation
 */
#define WATCH_PRIO      1                   /* Task priority */
#define WATCH_STK_SIZE  5*1024              /* Task stack size */
TaskHandle_t            WATCHTask_Handler;  /* Task handle */
void watch(void *pvParameters);             /* Task function */

SemaphoreHandle_t BinarySemaphore;          /* Binary semaphore handle */

/**
 * @brief       watch
 * @param       pvParameters : passed-in parameter (unused)
 * @retval      none
 */
void watch(void *pvParameters)
{
    pvParameters = pvParameters;

    while(1)
    {
        LED_TOGGLE();

        vTaskDelay(500);
    }
}

/**
 * @brief       lvgl_demo entry function
 * @param       none
 * @retval      none
 */
void lvgl_demo(void)
{
    while(images_init())    /* Initialize the image library */
    {
        while (sd_spi_init())   /* SD card not detected */
        {
            lcd_show_string(30, 50, 200, 16, 16, "SD Card Failed!", RED);
            vTaskDelay(200);
            lcd_show_string(30, 50, 200, 16, 16, "               ", WHITE);
            vTaskDelay(200);
        }

        sd_check_en = 1;        /* Detect the SD card */
        
        lcd_show_string(30, 50, 200, 16, 16, "image Error!", RED);
        images_update_image(30, 90, 16, (uint8_t *)"0:", RED);
        vTaskDelay(1000);
        lcd_clear(WHITE);
    }

    if (xl9555_key_scan(0) == KEY3_PRES)
    {
        images_update_image(30, 90, 16, (uint8_t *)"0:", RED);
    }
    
    /* Create the binary semaphore */
    BinarySemaphore = xSemaphoreCreateBinary();
    xGuiSemaphore = xSemaphoreCreateMutex();

    if (WATCHTask_Handler == NULL)
    {
        /* Create the WATCH task */
        xTaskCreatePinnedToCore((TaskFunction_t )watch,                 /* Task function */
                                (const char*    )"watch",               /* Task name */
                                (uint16_t       )WATCH_STK_SIZE,        /* Task stack size */
                                (void*          )NULL,                  /* Parameter passed to the task function */
                                (UBaseType_t    )WATCH_PRIO,            /* Task priority */
                                (TaskHandle_t*  )&WATCHTask_Handler,    /* Task handle */
                                (BaseType_t     ) 1);                   /* Core on which the task runs */
    }

    lv_init();              /* Initialize the LVGL graphics library */
    lv_port_disp_init();    /* Initialize the LVGL display interface; call after lv_init() */
    lv_port_indev_init();   /* Initialize the LVGL input interface; call after lv_init() */

    /* Provide the time base for LVGL */
    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 1 * 1000));

    lv_load_main_window();

    while (1)
    {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }

        vTaskDelay(pdMS_TO_TICKS(5));  /* Delay 5 ms */
    }
}

/**
 * @brief       Initialize and register the display device
 * @param       none
 * @retval      none
 */
void lv_port_disp_init(void)
{
    void *buf1 = NULL;
    void *buf2 = NULL;

    /* Initialize the LCD display device */

    /*-----------------------------
     * Create a drawing buffer
     *----------------------------*/
    /**
     * LVGL needs a buffer to draw widgets.
     * The buffer contents are then copied to the display device via its 'flush_cb' (display flush function).
     * The buffer size must be larger than one row of the display device.
     *
     * There are 3 buffer configurations:
     * 1. Single buffer:
     *      LVGL draws the display contents here and writes them to the display device.
     *
     * 2. Double buffer:
     *      LVGL draws the display contents into one of the buffers and writes it to the display device.
     *      DMA must be used to write the contents to be displayed into the buffer.
     *      While data is being sent from the first buffer, LVGL can draw the next part of the screen into the other buffer.
     *      This allows rendering and flushing to run in parallel.
     *
     * 3. Full-size double buffer
     *      Set two full-size, screen-sized buffers and set disp_drv.full_refresh = 1.
     *      LVGL then always provides the whole rendered screen via 'flush_cb'; you only need to change the frame buffer address.
     */
    /* Create a drawing buffer */
    buf1 = heap_caps_malloc(lcd_self.width * 10 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    /* Initialize the display buffer */
    static lv_disp_draw_buf_t disp_buf;                                 /* Structure holding display buffer information */
    lv_disp_draw_buf_init(&disp_buf, buf1, NULL, lcd_self.width * 10);  /* Initialize the display buffer */
    
    /* Register the display device in LVGL */
    static lv_disp_drv_t disp_drv;      /* Display device descriptor (display driver to register with the HAL, structures and callbacks that interact with the display) */
    lv_disp_drv_init(&disp_drv);        /* Initialize the display device */
    
    /* Set the display resolution
     * Dynamic detection is used here to support multiple ALIENTEK screens.
     * In real projects the screen size is usually fixed, so it can be set directly.
     */
    disp_drv.hor_res = lcd_self.width;
    disp_drv.ver_res = lcd_self.height;

    /* Copy the buffer contents to the display device */
    disp_drv.flush_cb = lvgl_disp_flush_cb;

    /* Set the display buffer */
    disp_drv.draw_buf = &disp_buf;

    /* Register the display device */
    lv_disp_drv_register(&disp_drv);
}

/**
* @brief    Flush the internal buffer contents to a specific area on the screen
* @note     DMA or any hardware can be used to accelerate this operation in the background,
*           but 'lv_disp_flush_ready()' must be called after flushing completes
* @param    disp_drv : display device
* @param    area : area to flush, containing the diagonal coordinates of the filled rectangle
* @param    color_map : color array
* @retval   none
*/
static void lvgl_disp_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    lcd_color_fill(area->x1,area->y1,area->x2,area->y2,(uint16_t*)color_map);
    /* Important!!! Notify the graphics library that flushing is complete */
    lv_disp_flush_ready(drv);
}

/**
 * @brief       Tell LVGL the elapsed time
 * @param       arg : passed-in parameter (unused)
 * @retval      none
 */
void increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds have elapsed */
    lv_tick_inc(1);
}

/**
 * @brief       Initialize and register the input device
 * @param       none
 * @retval      none
 */
void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /* Initialize the button driver */
    keypad_init();

    /* Register the keypad input device */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv);
}

/**
 * @brief       Initialize the keypad
 * @param       none
 * @retval      none
 */
static void keypad_init(void)
{
    
}

uint32_t g_last_key = 0;

/**
 * @brief       Graphics library keypad read callback
 * @param       indev_drv : keypad device
 *   @arg       data      : input device data structure
 * @retval      none
 */
static void keypad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    /* Get whether the button is pressed and save the key value */
    uint32_t act_key = keypad_get_key();

    if(act_key != 0) 
    {
        data->state = LV_INDEV_STATE_PR;

        /* Convert the key value to an LVGL control character */
        switch(act_key) 
        {
            case KEY0_PRES:
                act_key = LV_KEY_NEXT;
            break;

            case KEY1_PRES:
                act_key = KEY1_PRES;
                back_act_key = KEY1_PRES;
            break;

            case KEY2_PRES:
                act_key = LV_KEY_PREV;
            break;
            
            case KEY3_PRES:
                act_key = LV_KEY_ENTER;
            break;
        }

        g_last_key = act_key;
    } 
    else 
    {
        data->state = LV_INDEV_STATE_REL;
        g_last_key = 0;
    }

    if (back_act_key == KEY1_PRES && decode_en == ESP_OK)
    {
        if (app_obj_general.current_parent != NULL || app_obj_general.del_parent != NULL)
        {
            app_obj_general.Function();
        }

        back_act_key = 0;
    }

    data->key = g_last_key;
}

/**
 * @brief       Get the button currently being pressed
 * @param       none
 * @retval      0 : no button is pressed
 */
static uint32_t keypad_get_key(void)
{
    return xl9555_key_scan(0);
}
