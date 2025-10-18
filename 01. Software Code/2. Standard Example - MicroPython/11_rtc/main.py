"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    RTCexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：studyESP32-S3In the filmRTCThe use of

 * Hardware resources and pin allocation： 
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
 * 1, passLCDReal-time displayRTCtime。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,RTC,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 Initialization
    xl9555 = io_ex.init(i2c0)
    
    # ResetLCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    # initializationSPI
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # initializationLCD,lcd = 0for2.4inchScreen;lcd = 1for1.3inchSPILCDScreen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # Turn on backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # 显示experiment信息
    display.string(30, 550, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "RTC TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "Specific:", lcd.RED)
    display.string(30, 130, 200, 16, 16, "Time:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "Date:", lcd.RED)
    # Initialize RTC
    rtc = RTC()
    
    if rtc.datetime()[0] != 2023:
        rtc.datetime((2023, 8, 15, 2, 0, 0, 0, 0))
    
    while True:

        data_time = rtc.datetime()
        display.string(110, 110, 200, 16, 16, str(data_time[0:3]), lcd.RED)
        display.string(110, 130, 200, 16, 16, str(data_time[4:7]), lcd.RED)
        display.string(110, 150, 200, 16, 16, str(int(data_time[3]) + 1), lcd.RED)