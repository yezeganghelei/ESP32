/**
 ****************************************************************************************************
 * @file        xl9555.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       XL9555Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __XL9555_H
#define __XL9555_H

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "iic.h"

/* Pin and related parameter definitions */
#define XL9555_INT_IO               GPIO_NUM_40                     /* XL9555_INT pin */
#define XL9555_INT                  gpio_get_level(XL9555_INT_IO)   /* ReadXL9555_INTLevel of */

/* XL9555Command Macro */
#define XL9555_INPUT_PORT0_REG      0                               /* input register0address */
#define XL9555_INPUT_PORT1_REG      1                               /* input register1address */
#define XL9555_OUTPUT_PORT0_REG     2                               /* Output register 0 address */
#define XL9555_OUTPUT_PORT1_REG     3                               /* Output Register1address */
#define XL9555_INVERSION_PORT0_REG  4                               /* Polarity inversion register0address */
#define XL9555_INVERSION_PORT1_REG  5                               /* Polarity inversion register1address */
#define XL9555_CONFIG_PORT0_REG     6                               /* Direction Configuration Register 0 Address */
#define XL9555_CONFIG_PORT1_REG     7                               /* Direction Configuration Register1address */

#define XL9555_ADDR                 0X20                            /* XL9555 address (moved one by one left)-->Please see the manual (9.1. Device Address) */

/* Functions of each IO of XL9555 */
#define AP_INT_IO                   0x0001
#define QMA_INT_IO                  0x0002
#define SPK_EN_IO                   0x0004
#define BEEP_IO                     0x0008
#define OV_PWDN_IO                  0x0010
#define OV_RESET_IO                 0x0020
#define GBC_LED_IO                  0x0040
#define GBC_KEY_IO                  0x0080
#define LCD_BL_IO                   0x0100
#define CT_RST_IO                   0x0200
#define SLCD_RST_IO                 0x0400
#define SLCD_PWR_IO                 0x0800
#define KEY3_IO                     0x1000
#define KEY2_IO                     0x2000
#define KEY1_IO                     0x4000
#define KEY0_IO                     0x8000

#define KEY0                        xl9555_pin_read(KEY0_IO)        /* ReadKEY0Pin */
#define KEY1                        xl9555_pin_read(KEY1_IO)        /* ReadKEY1Pin */
#define KEY2                        xl9555_pin_read(KEY2_IO)        /* Read the KEY2 pin */
#define KEY3                        xl9555_pin_read(KEY3_IO)        /* Read the KEY3 pin */

#define KEY0_PRES                   1                               /* KEY0 Press */
#define KEY1_PRES                   2                               /* KEY1 Press */
#define KEY2_PRES                   3                               /* KEY1 Press */
#define KEY3_PRES                   4                               /* KEY1 Press */

/* Function declaration */
void xl9555_init(i2c_obj_t self);                                   /* Initialize XL9555 */
int xl9555_pin_read(uint16_t pin);                                  /* Get a certain IO status */
uint16_t xl9555_pin_write(uint16_t pin, int val);                   /* Control a certainIOLevel of */
esp_err_t xl9555_read_byte(uint8_t* data, size_t len);              /* ReadXL9555of16BitIOvalue */
uint8_t xl9555_key_scan(uint8_t mode);                              /* Scan buttonvalue */

#endif