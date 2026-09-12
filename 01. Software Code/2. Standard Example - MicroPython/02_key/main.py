"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Key input experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：studyGPIOUse as input

 * Hardware resources and pin assignments： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1
 * 2, KEY --> ESP32S3 IO
 *    KEY --> IO0

 * Experimental phenomenon
 * 1, pressBOOTPress buttons to controlLED0state flip。

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
if __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)          # Configurationledpin mode
    key = Pin(0,Pin.IN,Pin.PULL_UP)         # Configure key pin mode and pull-up
    
    while True:
        
        if key.value() == 0:                # judgeKEYWhether to press
            
            time.sleep_ms(10)               # This delay is for button debounce
            
            if key.value() == 0:            # Check again whether the button is pressed
                led_state = led.value()
                led.value(not led_state)
                
                while not key.value():      # Check whether the button is released
                    pass