"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    PWMexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：studyPWMOutput function

 * Hardware resources and pin allocation： 
 * 1, LED --> ESP32S3 IO
 *    LED --> IO1

 * experiment现象
 * 1, LED0From dark to bright，From bright to dark，Cycle in turn。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,PWM
import time

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
    
    pwm = PWM(Pin(1),freq = 1000)
    
    while True:
        # gradually brighten
        for i in range(0,1024):
            pwm.duty(i)
            time.sleep_ms(1)
            
        # Darker
        for i in range(1023,0,-1):
            pwm.duty(i)
            time.sleep_ms(1)