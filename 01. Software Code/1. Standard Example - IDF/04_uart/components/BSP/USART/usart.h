/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       Serial port initialization code(Generally serial port0)
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef _USART_H
#define _USART_H

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uart_select.h"
#include "driver/gpio.h"

/* Pin and serial port definitions */
#define USART_UX            UART_NUM_0
#define USART_TX_GPIO_PIN   GPIO_NUM_43
#define USART_RX_GPIO_PIN   GPIO_NUM_44

/* Related definitions for serial port reception */
#define RX_BUF_SIZE         1024    /* Ring buffer size */

/* Function declaration */
void usart_init(uint32_t baudrate); /* Initialize the serial port */

#endif