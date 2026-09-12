"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    LED experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: How to light up one LED

 * Hardware resources and pin allocation: 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1

 * Experiment phenomenon
 * 1, The LED flashes every 500 ms.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if  __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    
    while True:
        
        led.value(0)                    # Set GPIO1 output low level
        time.sleep_ms(500)              # Delay 500 ms
        led.value(1)                    # Set GPIO1 output high level
        time.sleep_ms(500)              # Delay 500 ms