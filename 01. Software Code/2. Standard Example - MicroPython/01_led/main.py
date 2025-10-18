"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    LEDexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：How to light up oneLED

 * Hardware resources and pin allocation： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1

 * experiment现象
 * 1, LEDEvery500msFlashing。

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
        
        led.value(0)                    # set upGPIO1Output low level
        time.sleep_ms(500)              # Delay500ms
        led.value(1)                    # Set GPIO1 output high level
        time.sleep_ms(500)              # Delay500ms