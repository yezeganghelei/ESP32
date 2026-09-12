"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    EEPROM experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: Study the use of IIC peripherals and read/write operations on the EEPROM
 *                     device

 * Hardware resources and pin allocation: 
 * 1,    KEY --> ESP32S3 IO
 *       KEY --> IO0
 * 2,  UART0 --> ESP32S3 IO
 *      TXD0 --> IO43
 *      RXD0 --> IO44
 * 3, EEPROM --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41

 * Experiment phenomenon
 * 1, In this experiment, KEY0 controls writing to the 24C02 and another button, KEY1, controls
 *    reading from the 24C02. The related information is displayed on the LCD module. At the same
 *    time, USMART can be used to write and read data at any address in the 24C02.

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
    # Initialize XL9555
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
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # Display experiment information
    display.string(0, 5, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "EEPORM TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "Data:", lcd.RED)
    # Initialize at24c02
    at24cx = at24c02.init(i2c0)
    time.sleep_ms(500) # Delay 500 ms
    
    # Check whether the device exists
    while at24cx.at24cxx_check():
        print("Device query failed！！！")
        time.sleep_ms(500)          # Delay 500 ms
    
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

        time.sleep_ms(10)          # Delay 10 ms