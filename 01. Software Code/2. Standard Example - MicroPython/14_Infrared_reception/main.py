"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Infrared remote control experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: Learn to use the infrared remote control

 * Hardware resources and pin assignments: 
 * 1,     LED --> ESP32S3 IO
 *        LED --> IO1
 * 2,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> IO0(Jumper cap connection) 
 * 3, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12
 * 4,    R_IN --> ESP32S3 IO
 *         IN --> IO2

 * Experiment phenomenon
 * 1, In this experiment, after some information is displayed on the LCD at power-on, the program
 *    waits for an infrared trigger. If the correct infrared signal is received, it is decoded and
 *    the key value, its meaning and the number of key presses are displayed on the LCD.
 * 2, The LED flashes, indicating that the program is running.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import remote
import time

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == "__main__":
    
    # Initialize LED
    led = Pin(1,Pin.OUT, value = 1)
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
    xl9555 = io_ex.init(i2c0)
    
    # Reset LCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    
    # Initialize SPI
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # Initialize LCD; lcd = 0 for a 2.4-inch screen, lcd = 1 for a 1.3-inch SPI LCD screen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # Experimental information
    display.string(30, 50, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 240, 16, 16, "REMOTE TEST",lcd.RED)
    display.string(30, 90, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 120, 200, 16, 16, "CMD:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "DATA:", lcd.RED)
    # Initialize infrared reception
    ir = remote.REMOTE_IR(2)
    
    while True :
        
        cmd,key_str = ir.remote_scan()
        display.fill(30 + 11 * 8 - 2,120,30 + 11 * 8 + 100,200,lcd.WHITE)
        display.string(30 + 11 * 8, 120, 200, 16, 16, str(cmd), lcd.BLUE)
        display.string(30 + 11 * 8, 150, 200, 16, 16, str(key_str), lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(500)         # Delay 500 ms