"""
 ******************************************************************************
 * @file main.py
 * @author team()
 * @version V1.0
 * @date 2023-12-01
 * @brief Infrared receiving and sending experiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************

 * Experiment purpose: learn the use of infrared receiving sensors

 *Hardware resources and pin allocation:
 * 1, LED --> ESP32S3 IO
 * LED --> IO1
 * 2, XL9555 --> ESP32S3 IO
 * SCL --> IO42
 * SDA --> IO41
 * INT --> IO0 (jumper cap connection)
 * 3, SPI_LCD --> ESP32S3 IO / XL9555
 * CS --> IO21
 * SCK --> IO12
 * SDA --> IO11
 * DC --> IO40 (jumper cap connection)
 * PWR --> XL9555_P13
 * RST --> XL9555_P12
 * 4, R_IN --> ESP32S3 IO
 * IN --> IO2
 * 5, R_OUT --> ESP32S3 IO
 * OUT --> IO8 (jumper cap connection)

 *Experimental phenomena
 * 1. It doesn’t matter if you don’t have an infrared remote control. The Pandora development board has an onboard infrared emission sensor. You can use the experiment in the next chapter.
 * Realize the spontaneous and self-receiving function of the infrared sensor of the development board.
 *2, LED flashes, indicating that the program is running.

 *Notes
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
from emission import NEC
import remote
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == "__main__":
    
    t = 0
    # initializationLED
    led = Pin(1,Pin.OUT, value = 1)
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
    spi = SPI(2,baudrate = 80000000, sck = Pin(12), mosi = Pin(11), miso = Pin(13))
    # initializationLCD,lcd = 0for2.4inchScreen;lcd = 1for1.3inchSPILCDScreen;
    display = lcd.init(spi,dc = Pin(40,Pin.OUT,Pin.PULL_UP,value = 1),cs = Pin(21,Pin.OUT,Pin.PULL_UP,value = 1),dir = 1,lcd = 0)
    # Turn on the backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # Experimental information
    display.string(30, 50, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 240, 16, 16, "REMOTE RX/TX TEST",lcd.RED)
    display.string(30, 90, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 120, 200, 16, 16, "RX CMD:", lcd.RED)
    display.string(30, 140, 200, 16, 16, "TX CMD:", lcd.RED)
    display.string(30, 160, 200, 16, 16, "DECODE:", lcd.RED)
    # Initialize infrared reception
    ir = remote.REMOTE_IR(2)
    # Initialize infrared emission
    irb = NEC(Pin(8, Pin.OUT, value = 0), 38000)
    
    while True :

        t += 1
        
        if t == 0:
            t = 1
        
        irb.transmit(0,t)
        cmd,s = ir.remote_scan()
        time.sleep_ms(200)         # delay100ms
        display.fill(30 + 11 * 8 - 2,120,30 + 11 * 8 + 100,200,lcd.WHITE)
        display.string(30 + 11 * 8, 120, 200, 16, 16, str(cmd), lcd.BLUE)
        display.string(30 + 11 * 8, 140, 200, 16, 16, str(t - 1), lcd.BLUE)
        display.string(30 + 11 * 8, 160, 200, 16, 16, str(s), lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(1000)         # delay1000ms