"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    IOExtended experiments
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：How to useIICdriveXL9555(16BitIOexpansion chip)

 * Hardware resources and pin assignments： 
 * 1,    LED --> ESP32S3 IO
 *       LED --> IO1
 * 2, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection)

 * experimental phenomenon
 * 1, User can pressKEY0~KEY3按键来controlLEDand buzzer，KEY0andKEY1来control蜂鸣器开与关；KEY2andKEY3controlLEDLight and extinguish。

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
            xl9555.write_bit(io_ex.BEEP,0)      # Open蜂鸣器
        elif key == io_ex.KEY1:
            xl9555.write_bit(io_ex.BEEP,1)      # Turn off the buzzer
        elif key == io_ex.KEY2:
            led.value(0)                        # Turn on LED
        elif key == io_ex.KEY3:
            led.value(1) 

        time.sleep_ms(10)                       # Delay 10ms