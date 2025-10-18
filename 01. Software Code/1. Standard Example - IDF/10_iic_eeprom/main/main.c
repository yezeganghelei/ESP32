/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file main.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief EEPORM experiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "led.h"
#include "iic.h"
#include "xl9555.h"
#include "24cxx.h"

i2c_obj_t i2c0_master;

const uint8_t g_text_buf[] = {"ESP32-S3 EEPROM"};   /* To write to24c02array of strings */
#define TEXT_SIZE   sizeof(g_text_buf)              /* TEXTString length */

/**
 * @brief display experiment information
 * @param None
 * @retval None
 */
void show_mesg(void)
{
    /* Serial port output experimental information */
    printf("\n");
    printf("********************************\n");
    printf("ESP32\n");
    printf("IIC EEPROM TEST\n");
    printf("ATOM@ALIENTEK\n");
    printf("KEY0:Write Data, KEY1:Read Data\n");
    printf("********************************\n");
    printf("\n");
}

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    uint16_t i = 0;
    uint8_t err = 0;
    uint8_t key;
    uint8_t datatemp[TEXT_SIZE];
    esp_err_t ret;
    
    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    led_init();                         /* initializationLED */
    i2c0_master = iic_init(I2C_NUM_0);  /* initializationIIC0 */
    xl9555_init(i2c0_master);           /* IO expansion chip initialization */
    at24cxx_init(i2c0_master);          /* initialization24CXX */
    show_mesg();                        /* Display experimental information */

    err = at24cxx_check();              /* TestAT24C02 */
    
    if (err != 0)
    {
        while (1)                       /* Not detected24c02 */
        {
            printf("24C02 check failed, please check!\n");
            vTaskDelay(500);
            LED_TOGGLE();               /* LED flashing */
        }
    }

    printf("24C02 Ready!\n");
    printf("\n");

    while(1)
    {
        key = xl9555_key_scan(0);
        
        switch (key)
        {
            case KEY0_PRES:
            {
                at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
                printf("The data written is:%s\n", g_text_buf);
                break;
            }
            case KEY1_PRES:
            {
                at24cxx_read(0, datatemp, TEXT_SIZE);
                printf("The data read is:%s\n", datatemp);
                break;
            }
            default:
            {
                break;
            }
        }

        i++;

        if (i == 20)
        {
            LED_TOGGLE();               /* LED flashing */
            i = 0;
        }
        
        vTaskDelay(10);
    }
}