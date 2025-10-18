/**
 ****************************************************************************************************
 * @file        ds18b20.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       DS18B20 driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __DS18B20_H
#define __DS18B20_H

#include "Arduino.h"
#include "esp_system.h"

/* Pin definition */
#define DS18B20_DQ_PIN      GPIO_NUM_0

/* Macro function */
#define DS18B20_DQ_OUT(x)   gpio_set_level(DS18B20_DQ_PIN, x)
#define DS18B20_DQ_IN       gpio_get_level(DS18B20_DQ_PIN)

#define DS18B20_MODE_IN     gpio_set_direction(DS18B20_DQ_PIN, GPIO_MODE_INPUT)
#define DS18B20_MODE_OUT    gpio_set_direction(DS18B20_DQ_PIN, GPIO_MODE_OUTPUT)

/* Function declaration */
uint8_t ds18b20_init(void);             /* DS18B20initialization */
uint8_t ds18b20_check(void);            /* Check if there isDS18B20 */
short ds18b20_get_temperature(void);    /* Get temperature */

#endif