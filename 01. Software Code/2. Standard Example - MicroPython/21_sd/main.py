"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    SDexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：How to driveSDCard，And realize read and write operations

 * Hardware resources and pin assignments： 
 * 1,   SD --> ESP32S3 IO
 *    SDCS --> IO2
 *     SCK --> IO12
 *    MOSI --> IO11
 *    MISO --> IO13

 * experiment现象
 * 1, 本experiment代码,首先读取mountSDSystem file directory before the card，mount成功之后再次读取系统文件目录，At this time, the system file directory containssdCard文件夹，Then rightSDRead and write files on the card。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
from sdcard import SDCard
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time
import uos

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
    
    x = 0
    # IIC initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 Initialization
    xl9555 = io_ex.init(i2c0)
    
    # resetLCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    
    # initializationSPI
    spi = SPI(2,baudrate = 24000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # initializationLCD,lcd = 0for2.4inchScreen;lcd = 1for1.3inchSPILCDScreen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # Turn on backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    sd = SDCard(spi,Pin(2,Pin.OUT))
    # Experimental information
    display.string(30, 50, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 80, 240, 24, 24, "SD TEST",lcd.RED)
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 130, 200, 16, 16, "File Read:", lcd.BLUE)
    # Hang to SD/sd
    uos.mount(sd,'/sd')
    # Re-query the system file directory
    print('mountSDThe system directory after:{}'.format(uos.listdir()))
    with open("/sd/test.txt", "w") as f:
            f.write(str("Hello ALIENTEK"))

    # fromsdRead in the card directoryhello.txtFile content
    with open("/sd/test.txt", "r") as f:
        # Print the read content
        data = f.read()
    
    display.string(130, 130, 200, 16, 16, str(data), lcd.BLUE)
    # UnloadSDCard  
    uos.umount('/sd')