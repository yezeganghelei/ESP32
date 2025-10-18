/**
 ****************************************************************************************************
 * @file        main.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       QMA6100PThree-axis accelerometer sensor experiment
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "iic.h"
#include "spi.h"
#include "xl9555.h"
#include "lcd.h"
#include "qma6100p.h"

i2c_obj_t i2c0_master;

/**
 * @brief       Show original data
 * @param       x, y : coordinate
 * @param       title: title
 * @param       val  : value
 * @retval      none
 */
void user_show_mag(uint16_t x, uint16_t y, char *title, float val)
{
    char buf[20];

    sprintf(buf,"%s%3.1f", title, val);                 /* Format output */
    lcd_fill(x + 30, y + 16, x + 160, y + 16, WHITE);   /* Clear last data(Maximum display20Characters,20*8=160) */
    lcd_show_string(x, y, 160, 16, 16, buf, BLUE);      /* Show string */
}

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    uint8_t t;
    qma6100p_rawdata_t xyz_rawdata;
    esp_err_t ret;
    
    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);  /* initializationIIC0 */
    spi2_init();                        /* Initialize SPI2 */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    lcd_init();                         /* Initialize LCD */
    qma6100p_init(i2c0_master);         /* Initialize a three-axis accelerometer */
    
    lcd_show_string(30, 50, 200, 16, 16, "ESP32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "QMA6100P TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, " ACC_X :", RED);
    lcd_show_string(30, 130, 200, 16, 16, " ACC_Y :", RED);
    lcd_show_string(30, 150, 200, 16, 16, " ACC_Z :", RED);
    lcd_show_string(30, 170, 200, 16, 16, " Pitch :", RED);
    lcd_show_string(30, 190, 200, 16, 16, " Roll  :", RED);

    while (1)
    {
        vTaskDelay(10);
        t++;

        if (t == 20)                    /* 0.2Update three-axis original one in secondsvalue */
        {   
            qma6100p_read_rawdata(&xyz_rawdata);
            
            user_show_mag(30, 110, "ACC_X :", xyz_rawdata.acc_x);
            user_show_mag(30, 130, "ACC_Y :", xyz_rawdata.acc_y);
            user_show_mag(30, 150, "ACC_Z :", xyz_rawdata.acc_z);
            user_show_mag(30, 170, "Pitch :", xyz_rawdata.pitch);
            user_show_mag(30, 190, "Roll  :", xyz_rawdata.roll);
            
            t = 0;
            LED_TOGGLE();
        }
    }
}