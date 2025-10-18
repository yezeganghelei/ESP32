/**
 ****************************************************************************************************
 * @file        key.c
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Button driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "key.h"

/**
 * @brief initialization button
 * @param none
 * @retval None
 */
void key_init(void)
{
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* Input mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* Disabled pull-down */
    gpio_init_struct.pin_bit_mask = 1ull << BOOT_GPIO_PIN;  /* BOOTButton pin */
    gpio_config(&gpio_init_struct);                         /* Configuration enable */
}

/**
 * @brief       Key scan function
 * @param       mode:0 / 1, The specific meaning is as follows:
 *              0,  Continuous pressing is not supported(When the button is pressed and not put, 只有第一次调用会返回key value,
 *                  Must be loosened later, 再次Press才会返回其他key value)
 *              1,  Support continuous press(When the button is pressed and not put, 每次调用该函数都会返回key value)
 * @retval      key value, Definition is as follows:
 *              BOOT_PRES, 1, BOOTPress
 */
uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    static uint8_t key_boot = 1;    /* button release sign */

    if(mode)
    {
        key_boot = 1;
    }

    if (key_boot && (BOOT == 0))    /* The key release sign is1，and any button is pressed */
    {
        vTaskDelay(10);             /* debounce */
        key_boot = 0;

        if (BOOT == 0)
        {
            keyval = BOOT_PRES;
        }
    }
    else if (BOOT == 1)
    {
        key_boot = 1;
    }

    return keyval;                  /* 返回key value */
}