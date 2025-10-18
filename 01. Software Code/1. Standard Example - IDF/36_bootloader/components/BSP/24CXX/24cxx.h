/**
 ******************************************************************************************************
 * @file 24cxx.h
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief 24CXX driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 ******************************************************************************************************
 */
 
#ifndef __24CXX_H
#define __24CXX_H

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "iic.h"

/* 24c02 device address */
#define AT_ADDR     (0x50)

#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767

/* The development board uses 24c02, so EE_TYPE is defined as AT24C02 */
#define EE_TYPE     AT24C02

/* function declaration */
void at24cxx_init();                                               /* Initialize IIC */
uint8_t at24cxx_check(void);                                        /* Check the device */
uint8_t at24cxx_read_one_byte(uint16_t addr);                       /* Read a byte byte by specified address */
void at24cxx_write_one_byte(uint16_t addr,uint8_t data);            /* Write a byte to the specified address */
void at24cxx_write(uint16_t addr, uint8_t *pbuf, uint16_t datalen); /* Write data of specified length starting from specified address */
void at24cxx_read(uint16_t addr, uint8_t *pbuf, uint16_t datalen);  /* Read data of a specified length from the specified address */

#endif