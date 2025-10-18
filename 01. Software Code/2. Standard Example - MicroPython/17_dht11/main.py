"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    DHT11experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：ReadDS18B20The temperature and humidity of the sensor

 * Hardware resources and pin assignments： 
 * 1, LED --> ESP32S3 IO
 *      LED --> IO1
 * 2, XL9555-->ESP32S3 IO
 *      INT-->IO40
 *      SDA-->IO41
 *      CLK-->IO42
 * 3, SPILCD-->ESP32S3 IO
 *      MOSI-->IO11
 *      SCK-->IO12
 *      CS-->IO21
 *      DC-->IO40(Jumper cap connection)
 *      RST-->IOExpand10(SLCD_RST)
 *      PWR-->IOExpand11(SLCD_PWR)
 * 4, DHT11-->ESP32S3 IO
 *      DQ --> IO0(Jumper cap connection)

 * experiment现象
 * 1, LCDThe display updates the temperature and humidity values ​​in real time。
 * 2, LEDflashing，Prompt the program to run。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time
import dht

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':

    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC initialization
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
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # 提示experiment信息
    display.string(5, 10, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(5, 43, 240, 24, 24, "DHT11 TEST",lcd.RED)
    display.string(5, 68, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(5, 84, 200, 16, 16, "temperature: 00.00C", lcd.RED)
    display.string(5, 100, 200, 16, 16, "humidity: 00.00H", lcd.RED)
    # Create dht11 object
    dht11 = dht.DHT11(Pin(0, Pin.IN, Pin.PULL_UP))
    
    while True:
        
        # Measurement data
        dht11.measure()
        # Extract temperature data
        temperature = float(dht11.temperature())
         # Extract humidity data
        humidity = float(dht11.humidity())

        display.num(5 + 13 * 8,84,int(temperature),2,16,lcd.BLUE)
        display.num(5 + 16 * 8,84,int(temperature * 100 % 100),2,16,lcd.BLUE)
        display.num(5 + 10 * 8,100,int(humidity),2,16,lcd.BLUE)
        display.num(5 + 13 * 8,100,int(humidity * 100 % 100),2,16,lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)