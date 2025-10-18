/**
 ****************************************************************************************************
 * @file        key.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Button driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __KEY_H_
#define __KEY_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

/* Pin definition */
#define BOOT_GPIO_PIN   GPIO_NUM_0

/*IO operations*/
#define BOOT            gpio_get_level(BOOT_GPIO_PIN)

/* Press the button定义 */
#define BOOT_PRES       1       /* BOOTPress the button */

/* Function declaration */
void key_init(void);            /* Initialize button */
uint8_t key_scan(uint8_t mode); /* Key scan function */

#endif