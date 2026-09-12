"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    IO expansion experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: How to use IIC to drive the XL9555 (16-bit IO expansion chip)

 * Hardware resources and pin assignments: 
 * 1,    LED --> ESP32S3 IO
 *       LED --> IO1
 * 2, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection)

 * Experimental phenomenon
 * 1, The user can press KEY0~KEY3 to control the LED and buzzer. KEY0 and KEY1 turn the buzzer on
 *    and off; KEY2 and KEY3 turn the LED on and off.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,I2C
import atk_xl9555 as io_ex
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
    xl9555 = io_ex.init(i2c0)
    
    while True:
        
        # Get key value
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:
            xl9555.write_bit(io_ex.BEEP,0)      # Turn on the buzzer
        elif key == io_ex.KEY1:
            xl9555.write_bit(io_ex.BEEP,1)      # Turn off the buzzer
        elif key == io_ex.KEY2:
            led.value(0)                        # Turn on LED
        elif key == io_ex.KEY3:
            led.value(1) 

        time.sleep_ms(10)                       # Delay 10 ms