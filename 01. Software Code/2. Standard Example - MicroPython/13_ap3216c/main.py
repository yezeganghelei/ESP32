"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    AP3216C light and proximity sensor experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment: Learn to use the AP3216C light environment sensor and measure ambient
 *                           light intensity (ALS), proximity (PS) and infrared light intensity (IR).

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
 * 4, AP3216C --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> XL9555_P00

 * Experimental phenomenon
 * 1, This experiment detects whether the AP3216C exists at power-on. If it is not detected, an error
 *    message is displayed on the LCD. If it is detected, normal operation is shown and the main loop
 *    repeatedly reads the ALS+PS+IR sensor data and displays it on the LCD screen. At the same time,
 *    DS0 flashes, indicating that the program is running. In addition, this routine exposes the
 *    AP3216C read/write functions to USMART control, so the AP3216C can also be controlled through
 *    USMART.
 * 2, The LED flashes, indicating that the program is running.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import atk_ap3216 as ap3216c
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
    display.string(30, 50, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "AP3216C TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "ir:", lcd.RED)
    display.string(30, 130, 200, 16, 16, "ps:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "als:", lcd.RED)
    # Initialize AP3216C
    ap3216 = ap3216c.init(i2c0)

    while True:
        # Get data
        data = tuple(ap3216.ap3216c_read())
        # Convert data
        ir = (data[1] << 8) | data[0]
        ps = (data[3] << 8) | data[2]
        als = (data[5] << 8) | data[4]
        # Display the conversion result
        display.num(110, 110,int(ir),5,16,lcd.BLUE)
        display.num(110, 130,int(ps),5,16,lcd.BLUE)
        display.num(110, 150,int(als),5,16,lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(500)         # Delay 500 ms