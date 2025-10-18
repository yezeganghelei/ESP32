"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Timer interrupt experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：Learning timer interrupt

 * Hardware resources and pin allocation： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1

 * experimental phenomenon
 * 1, Timer every1second controlLEDstate flip。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,Timer
import time

"""
 * @brief       Basic timerTIMEXInterrupt service function
 * @param       tim:timer handle
 * @retval      none
"""
def BTMR_TIMEX_INT_IRQHandler(tim):
    led_state = led.value()
    led.value(not led_state)

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # Turn on the timer1
    tim = Timer(1)
    # Configure timer1：1000msinterrupt、Loop mode and interrupt callback functionBTMR_TIMEX_INT_IRQHandler
    tim.init(period = 1000, mode = Timer.PERIODIC,callback = BTMR_TIMEX_INT_IRQHandler)