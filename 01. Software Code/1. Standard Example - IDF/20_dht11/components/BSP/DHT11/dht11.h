/**
 ****************************************************************************************************
 * @file        dht11.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       DHT11Digital temperature and humidity sensor driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __DHT11_H
#define __DHT11_H

#include <driver/rmt.h>
#include <soc/rmt_reg.h>
#include "driver/gpio.h" 
#include <esp_log.h>

/* Pin definition */
#define DHT11_DQ_GPIO_PIN       GPIO_NUM_0

/* DHT11Pin high and low level enumeration */
typedef enum 
{
    DHT11_PIN_RESET = 0u,
    DHT11_PIN_SET
}DHT11_GPIO_PinState;

/* IOoperate */
#define DHT11_DQ_IN     gpio_get_level(DHT11_DQ_GPIO_PIN)   /* Data port input */

/* DHT11Port definition */
#define DHT11_DQ_OUT(x) do{ x ?                                                 \
                            gpio_set_level(DHT11_DQ_GPIO_PIN, DHT11_PIN_SET) :  \
                            gpio_set_level(DHT11_DQ_GPIO_PIN, DHT11_PIN_RESET); \
                        }while(0)

/* Function declaration */
void dht11_reset(void);                                 /* Reset DHT11 */
uint8_t dht11_init(void);                               /* initializationDHT11 */
uint8_t dht11_check(void);                              /* waitDHT11Response */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);   /* Read temperature and humidity */

#endif