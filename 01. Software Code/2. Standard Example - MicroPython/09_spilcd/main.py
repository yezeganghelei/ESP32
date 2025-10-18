"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    SPI LCD experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：How to drive1.3inch或2.4inchLCDscreen

 * Hardware resources and pin assignments： 
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

 * experiment现象
 * 1, LCD显示experiment信息，并Refresh color。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':
    
    x = 0
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 Initialization
    xl9555 = io_ex.init(i2c0)
    
    # Reset LCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    
    # Initialize SPI
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # Initialize LCD, lcd = 0 for 2.4-inch screen; lcd = 1 for 1.3-inch SPILCD screen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    
    While True:
        
        #Create a dictionary
        seasondict = {
        0: lcd.BLACK,
        1: lcd.BLUE,
        2: lcd.RED,
        3: lcd.GREEN,
        4: lcd.CYAN,
        5: lcd.MAGENTA,
        6: lcd.YELLOW}

        #Refresh color
        display.clear(seasondict[x])
        #Show fonts
        display.string(0, 5, 240, 32, 32, "ESP32-S3",lcd.RED)
        display.string(0, 34, 240, 16, 16, "SPI LCD Test",lcd.RED)
        display.string(0, 50, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
        x += 1
        
        if x == 7:
            x = 0
        time.sleep(1)