"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Watchdog experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: Learn the use of the independent watchdog

 * Hardware resources and pin assignments: 
 * 1,    LED --> ESP32S3 IO
 *       LED --> IO1
 * 2, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection)

 * Experimental phenomenon
 * 1, If the watchdog is not reset, the development board LED stays on. Pressing KEY0 feeds the dog;
 *    as long as KEY0 is held down, the watchdog never generates a reset and the LED stays on. Once
 *    the watchdog timeout is exceeded without a press, the program restarts, which turns the LED off
 *    once and looks like the LED is flashing.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,I2C,WDT
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
    time.sleep_ms(100)
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
    xl9555 = io_ex.init(i2c0)
    # Watchdog initialization, timeout is 3 s
    wdt = WDT(timeout=3000)
    led.value(0)
    
    while True:
        
        # Get key value
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:
            # Feed the dog
            wdt.feed()

        time.sleep_ms(10)                       # Delay 10 ms