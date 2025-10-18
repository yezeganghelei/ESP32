/**
 ****************************************************************************************************
 * @file        dht11.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       DHT11 Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __DHT11_H
#define __DHT11_H

#include "Arduino.h"
#include "esp_system.h"

/* Pin definition */
#define DHT11_DQ_PIN      GPIO_NUM_0       

/* Macro function definition */
#define DHT11_DQ_OUT(x)   gpio_set_level(DHT11_DQ_PIN, x)       //digitalWrite(DHT11_DQ, x) 
#define DHT11_DQ_IN       gpio_get_level(DHT11_DQ_PIN)          //digitalRead(DHT11_DQ)

#define DHT11_MODE_IN     gpio_set_direction(DHT11_DQ_PIN, GPIO_MODE_INPUT)   //pinMode(DHT11_DQ, INPUT_PULLUP)
#define DHT11_MODE_OUT    gpio_set_direction(DHT11_DQ_PIN, GPIO_MODE_OUTPUT)  //pinMode(DHT11_DQ, OUTPUT)

/* function declaration */
uint8_t dht11_init(void);                               /* dht11initialization function */
uint8_t dht11_check(void);                              /* Detect whether DHT11 exists */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi);  /* Read temperature and humidity */

#endif