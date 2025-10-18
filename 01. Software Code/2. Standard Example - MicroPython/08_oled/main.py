"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    OLEDexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experiment目的：studyOLEDshow屏的使用

 * Hardware resources and pin assignments： 
 * 1, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection) 
 * 2,   OLED --> ESP32S3 IO
 *   D0(SCL) --> IO40
 *   D1(SDA) --> IO41
 *        D2 --> IO6
 *        DC --> IO38

 * experiment现象
 * 1.ByOLEDInsert the module into the lower left corner of the development boardOLED/CAMERAmodule接口，
 *   After successful download，按下reset之后，You can seeOLEDmodule不停的showASCIIcode and code value。
 * 2.LEDflashing ,Prompt the program to run。

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,I2C
import atk_xl9555 as io_ex
import atk_oled as oled
import time

"""
 * @brief program entry
 * @param none
 * @retval None
"""
if __name__ == '__main__':
    
    # Initialize the LED and output high level
    led = Pin(46,Pin.OUT,value = 1)
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    i2c1 = I2C(1, scl = Pin(4), sda = Pin(5), freq = 400000)
    # start：The following is usedOLEDmoduleIICNecessary settings for communication,If using four-wireOLEDmodule，Can be deletedstart~endarea code
    xl9555 = io_ex.init(i2c0)
    dc = Pin(38,Pin.OUT,value = 0)

    # resetOLED
    xl9555.write_bit(io_ex.OV_RESET,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET,1)
    time.sleep_ms(100)
    # end
    
    # initializationOLED,Default settings：oled.init(port = 1,sda = 5,scl = 4,freq = 200000)
    display = oled.init(i2c1)
    
    # showexperiment信息
    display.string(0,0,str("ALIENTEK"),24)
    display.string(0,24,str("0.96' OLED TEST"),16)
    display.string(0,40,str("ATOM 2023/09/13"),12)
    display.string(0,52,str("ASCII:"),12)
    display.string(64,52,str("CODE:"),12)
    display.refresh_gram()
    
    t = ' '
    
    While True:
        
        # Show ASCII characters
        display.char(36,52,t,12,1)
        # Display the code value of ASCII characters
        display.num(94,52,ord(t),3,12)
        # Update to display to OLED
        display.refresh_gram()

        t = chr(ord(t) + 1)

        if t > '~':
        
            t = ' '
        
        time.sleep_ms(500)