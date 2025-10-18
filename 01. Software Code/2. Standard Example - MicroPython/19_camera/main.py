"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Camera experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：How to driveOV2640andOV5640camera module

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
 * 4,  CAMERA --> ESP32S3 IO / XL9555
 *     OV_SCL --> 38
 *     OV_SDA --> 39
 *      VSYNC --> 47
 *       HREF --> 48
 *       PCLK --> 45
 *         D0 --> 4
 *         D1 --> 5
 *         D2 --> 6
 *         D3 --> 7
 *         D4 --> 15
 *         D5 --> 16
 *         D6 --> 17
 *         D7 --> 18
 *      RESET --> XL9535_P05
 *       PWDN --> XL9535_P04

 * Experimental phenomenon
 * 1, This experimental code,开机的时候先initializationXL9555IOExpanding chip，然后Reset并Turn on the camera，Then check the camera andLCD进行initialization，Finally, call the function to display the camera data toLCDon the display。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

import time
import camera
import atk_xl9555 as io_ex
import atk_lcd as lcd
from machine import Pin,SPI,I2C

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':
    
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 Initialization
    xl9555 = io_ex.init(i2c0)
    xl9555.write_bit(io_ex.BEEP,1)
    
    # Reset摄像头
    xl9555.write_bit(io_ex.OV_RESET,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET,1)
    time.sleep_ms(100)
    # Turn on the camera
    xl9555.write_bit(io_ex.OV_PWDN,1)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_PWDN,0)
    time.sleep_ms(100)
    # 给Reset留时间
    time.sleep_ms(1000)
    
    # initialization摄像头
    for i in range(5):
        cam = camera.init(0, format=camera.RGB565, fb_location=camera.PSRAM,framesize = camera.FRAME_240X240,xclk_freq = 24000000)
        print("Camera ready?: ", cam)
        if cam:
            print("Camera ready")
            break
        else:
            camera.deinit()
            camera.init(0, format=camera.RGB565, fb_location=camera.PSRAM,framesize = camera.FRAME_240X240,xclk_freq = 24000000)
            time.sleep(2)
    else:
        print('Timeout')
        reset()
    
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
    display.camera(42,5)