"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    APC3216CLight and proximity sensor experiments
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment：Learning Light Environment SensorsAP3216CUse,Realize light intensity(ALS)/close distance(PS)/Infrared light intensity(IR)Measurement of etc.

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
 * 4, AP3216C --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> XL9555_P00

 * Experimental phenomenon
 * 1, This experiment,Detect before powering onAP3216Cexists，If not detectedAP3216C，ThenLCDAn error message is displayed on the screen。If check
 *    DetectedAP3216C，It will show normal，并在主循环里noodle，Loop readingALS+PS+IRSensor data，And display inLCDScreen上
 *    noodle。at the same time，DS0Flashing，Prompt the program is running。in addition，This routine willAP3216CThe read and write operation functions are addedUSMARTcontrol，
 *    我们也可以通过USMARTrightAP3216C进行control。。
 * 2, LEDFlashing，Prompt the program to run。

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
    # Turn on backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # Show experiment information
    display.string(30, 50, 240, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "AP3216C TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "ir:", lcd.RED)
    display.string(30, 130, 200, 16, 16, "ps:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "als:", lcd.RED)
    # initializationAP1632C
    ap3216 = ap3216c.init(i2c0)

    while True:
        # 获取数据
        data = tuple(ap3216.ap3216c_read())
        # Convert data
        ir = (data[1] << 8) | data[0]
        ps = (data[3] << 8) | data[2]
        als = (data[5] << 8) | data[4]
        # Delay conversion result
        display.num(110, 110,int(ir),5,16,lcd.BLUE)
        display.num(110, 130,int(ps),5,16,lcd.BLUE)
        display.num(110, 150,int(als),5,16,lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(500)         # Delay500ms