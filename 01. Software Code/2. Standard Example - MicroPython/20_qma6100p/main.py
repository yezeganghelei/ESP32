"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    QMA6100PThree-axis accelerometer sensor experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：studyQMA6100PThree-axis accelerometer and other measurements

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
 * 4, QMA6100P--> ESP32S3 IO
 *        SCL --> IO40
 *        SDA --> IO41
 *        INT --> XL9555_P01

 * experiment现象
 * 1, 本experiment,Test first when starting the machineQMA6100PDoes it exist，If not detectedAP3216C，Then the error message is displayed on the monitor。If checked
 *    MeasuredQMA6100P，It will show normal，And inside the main loop，Loop reading of three axesXYZRaw data and pitch angle、Rolling angle、Acceleration sensor data，And display inLCDon the screen。
 * 2, LEDflashing，Prompt the program to run。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,SPI,I2C
import atk_xl9555 as io_ex
import atk_lcd as lcd
import atk_qma6100p as qma6100p
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
    # Turn on LCD backlight
    xl9555.write_bit(io_ex.SLCD_PWR,1)
    time.sleep_ms(100)
    # Initialize qma6100p
    qma6100 = qma6100p.init(i2c0)
    
    # 显示experiment信息
    display.string(30, 50, 200, 16, 16, "ESP32-S3",lcd.RED)
    display.string(30, 70, 200, 16, 16, "QMA6100P TEST", lcd.RED)
    display.string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", lcd.RED)
    display.string(30, 110, 200, 16, 16, "acc_x:", lcd.RED)
    display.string(30, 130, 200, 16, 16, "acc_y:", lcd.RED)
    display.string(30, 150, 200, 16, 16, "acc_z:", lcd.RED)
    display.string(30, 170, 200, 16, 16, "acc_g:", lcd.RED)
    display.string(30, 190, 200, 16, 16, "pitch:", lcd.RED)
    display.string(30, 210, 200, 16, 16, "roll:", lcd.RED)
    
    while True :
        
        qma6100.qma6100p_read()
        display.string(110, 110, 200, 16, 16,str(qma6100.qma6100p_acc_x()),lcd.BLUE)
        display.string(110, 130, 200, 16, 16,str(qma6100.qma6100p_acc_y()),lcd.BLUE)
        display.string(110, 150, 200, 16, 16,str(qma6100.qma6100p_acc_z()),lcd.BLUE)
        display.string(110, 170, 200, 16, 16,str(qma6100.qma6100p_acc_g()),lcd.BLUE)
        display.string(110, 190, 200, 16, 16,str(qma6100.qma6100p_acc_pitch()),lcd.BLUE)
        display.string(110, 210, 200, 16, 16,str(qma6100.qma6100p_acc_roll()),lcd.BLUE)
        led_state = led.value()
        led.value(not led_state)
        time.sleep_ms(100)         # Delay100ms