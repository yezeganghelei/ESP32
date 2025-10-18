"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    ADCexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：studyADCUse

 * 硬件资源及Pin分配： 
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
 * 4,      RV --> ESP32S3 IO
 *     ADC_IN --> IO8(Jumper cap connection)

 * experiment现象
 * 1, ADCAcquisition channel（IO8）voltage on，And inLCDShown onADCThe converted digital quantity of the voltage and the converted analog quantity。
 * 2, LEDflashing，Prompt the program to run。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,ADC,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import time

"""
 * @brief       ADC取平均value
 * @param       times：frequency
 * @retval      return：ADC平均value
"""
def adc_get_result_average(times):
    
    temp_val = 0
    
    for i in range(0,times):
        temp_val += adc.read()
        
    return temp_val / times

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
    
    adcdata = 0
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
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
    display.string(30, 50, 240, 32, 32, "ESP32-S3",lcd.RED)
    display.string(30, 80, 240, 24, 24, "ADC TEST",lcd.RED)
    display.string(30, 110, 240, 16, 16, "ATOM@ALIENTEK",lcd.RED)
    display.string(30, 130, 200, 16, 16, "ADC:  0 . 0    V", lcd.BLUE)
    # Initialize ADC
    adc = ADC(Pin(8))         # Pin8Connect to the potentiometer on the bottom panel
    adc.atten(ADC.ATTN_11DB)
    adc.width(ADC.WIDTH_12BIT)  #4095
    
    while True:

        # readADCvalue
        adcdata = adc_get_result_average(20)
        # readADCvalue
        umber = float(adcdata * (3.3 / 4096))
        display.num(30 + 40 ,130,int(umber),2,16,lcd.RED)
        display.num(30 + 72 ,130,int(umber * 100 % 100),2,16,lcd.RED)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(100)