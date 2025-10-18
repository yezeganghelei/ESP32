/**
 ****************************************************************************************************
 * @file        tud_usart.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       USBSimulated serial port code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __TUD_USART_H
#define __TUD_USART_H

#include <inttypes.h>
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#include "esp_log.h"

/* Function declaration */
void tud_usb_usart(void);

#endif