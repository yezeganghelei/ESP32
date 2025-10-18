"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    EEPORMexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：studyIICUse of peripherals，rightEEPROMRead and write operations of the device

 * Hardware resources and pin allocation： 
 * 1,    KEY --> ESP32S3 IO
 *       KEY --> IO0
 * 2,  UART0 --> ESP32S3 IO
 *      TXD0 --> IO43
 *      RXD0 --> IO44
 * 3, EEPROM --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41

 * experiment现象
 * 1, 本experiment通过KEY0按键to control24C02writing，via another buttonKEY1to control24C02of reading。And inLCDDisplayed on the module
 *    Related information。at the same time，we can passUSMARTcontrolled in24C02Write and read data from any address。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import atk_at24cx as at24c02
import time

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
    
    var1 = 'Hello ALIENTEK ESP32-S3'
    # IIC initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # initializationXL9555
    xl9555 = io_ex.init(i2c0)
    
    # resetLCD
    xl9555.write_bit(io_ex.SLCD_RST,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.SLCD_RST,1)
    time.sleep_ms(100)
    # initializationSPI
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # initializationLCD,lcd = 0for2.4inchScreen;lcd = 1for1.3inchSPILCDScreen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # 显示experiment信息
    display.string(0, 5, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "EEPORM TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "Data:", lcd.RED)
    # Initialize atc02
    at24cx = at24c02.init(i2c0)
    time.sleep_ms(500) # Delay 500ms
    
    # Check whether the device exists
    while at24cx.at24cxx_check():
        print("Device query failed！！！")
        time.sleep_ms(500)          # Delay500ms
    
    while True:
        
        # Get key value
        key = int(xl9555.key_scan())
        
        if key == io_ex.KEY0:
            # Write data to the device
            at24cx.at24cxx_write(0,var1,len(var1))
        elif key == io_ex.KEY1:
            # Read data
            data = str(at24cx.at24cxx_read(0,len(var1)))
            display.string(110, 110, 200, 16, 16, str(data), lcd.BLUE)

        time.sleep_ms(10)          # Delay 10ms