/**
 ******************************************************************************
 * @file     main.c
 * @author   wocream team
 * @version  V1.0
 * @date     2023-08-26
 * @brief    LED
 ******************************************************************************
 */

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "led.h"
#include "usart.h"

/**
 * @brief       main interface
 * @param       None
 * @retval      None
 */
void app_main(void)
{
    esp_err_t ret;
    uint8_t len = 0;
    uint16_t times = 0;
    unsigned char data[RX_BUF_SIZE] = {0};

    ret = nvs_flash_init();                                                             /* initializationNVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    led_init();                                                                         /* Initialize LED */
    usart_init(115200);                                                                 /* Initialize the serial port */

    while(1)
    {
        uart_get_buffered_data_len(USART_UX, (size_t*) &len);                           /* Get the length of the ring buffer data */

        if (len > 0)                                                                    /* Determine the data length */
        {
            memset(data, 0, RX_BUF_SIZE);                                               /* Clear the buffer */
            printf("\nsend msg:\n");
            uart_read_bytes(USART_UX, data, len, 100);                                  /* Read data */
            uart_write_bytes(USART_UX, (const char*)data, strlen((const char*)data));   /* Write data */
        }
        else
        {
            times++;

            if (times % 5000 == 0)
            {
                printf("\n ATK-DNESP32-S3 development board\n");
            }

            if (times % 200 == 0)
            {
                printf("input data，enter to end\n");
            }

            if (times % 30 == 0)
            {
                LED_TOGGLE();
            }

            vTaskDelay(10);
        }
    }
}