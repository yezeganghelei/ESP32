/**
 ****************************************************************************************************
 * @file        ds18b20.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       DS18B20Digital temperature sensor drive code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __DS18B20_H
#define __DS18B20_H

#include "driver/gpio.h" 
#include <esp_log.h>

/* Pin definition */
#define DS18B20_DQ_GPIO_PIN       GPIO_NUM_0

/* DS18B20 pin high and low level enumeration */
typedef enum 
{
    DS18B20_PIN_RESET = 0u,
    DS18B20_PIN_SET
}DS18B20_GPIO_PinState;

/* IOoperate */
#define DS18B20_DQ_IN     gpio_get_level(DS18B20_DQ_GPIO_PIN)   /* Data port input */

/* DS18B20 port definition */
#define DS18B20_DQ_OUT(x) do{ x ?                                                   \
                            gpio_set_level(DS18B20_DQ_GPIO_PIN, DS18B20_PIN_SET) :  \
                            gpio_set_level(DS18B20_DQ_GPIO_PIN, DS18B20_PIN_RESET); \
                        }while(0)

/* Function declaration */
void ds18b20_reset(void);               /* ResetDS18B20 */
uint8_t ds18b20_check(void);            /* Detect whether it existsDS18B20 */
uint8_t ds18b20_read_bit(void);         /* Read a bit from DS18B20 */
uint8_t ds18b20_read_byte(void);        /* fromDS18B20Read a byte */
void ds18b20_write_byte(uint8_t data);  /* Write a byte toDS18B20 */
void ds18b20_start(void);               /* Start the temperature conversion */
uint8_t ds18b20_init(void);             /* initializationDS18B20 */
short ds18b20_get_temperature(void);    /* Get the temperature */

#endif