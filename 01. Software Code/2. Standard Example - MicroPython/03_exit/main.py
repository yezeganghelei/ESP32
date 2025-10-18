"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    External interrupt experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：Learn the use of external interrupts

 * Hardware resources and pin assignments： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1
 * 2, KEY --> ESP32S3 IO
 *    KEY --> IO0

 * experimental phenomenon
 * 1, PressBOOTButton controllableLED0state flip。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin
import time

"""
 * @brief       Key interrupt service function
 * @param       key:timer handle
 * @retval      none
"""
def KEY_INT_IRQHandler(key):
    
    time.sleep_ms(10) #Button debounce
    
    if key.value() == 0:
        global led_state
        led_state = led.value()
        led.value(not led_state)

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # ConfigurationkeyPin mode and pull-up and pull-down
    key = Pin(0,Pin.IN,Pin.PULL_UP)
    # Define interrupt，Falling edge trigger
    key.irq(KEY_INT_IRQHandler,Pin.IRQ_FALLING)
    # main loop，Prevent program from exiting
    while True:
        pass