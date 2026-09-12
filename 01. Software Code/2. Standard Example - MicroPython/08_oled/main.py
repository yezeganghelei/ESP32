"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    OLED experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose: Study the use of the OLED display screen

 * Hardware resources and pin assignments: 
 * 1, XL9555 --> ESP32S3 IO
 *       SCL --> IO42
 *       SDA --> IO41
 *       INT --> IO0(Jumper cap connection) 
 * 2,   OLED --> ESP32S3 IO
 *   D0(SCL) --> IO40
 *   D1(SDA) --> IO41
 *        D2 --> IO6
 *        DC --> IO38

 * Experiment phenomenon
 * 1, Plug the OLED module into the OLED/CAMERA module interface at the lower left corner of the
 *    development board. After a successful download, press reset and you will see the OLED module
 *    continuously displaying ASCII characters and their code values.
 * 2, The LED flashes, indicating that the program is running.

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
    # start: The following are the necessary settings for IIC communication with the OLED module. If using a four-wire OLED module, the code between start and end can be deleted
    xl9555 = io_ex.init(i2c0)
    dc = Pin(38,Pin.OUT,value = 0)

    # Reset OLED
    xl9555.write_bit(io_ex.OV_RESET,0)
    time.sleep_ms(100)
    xl9555.write_bit(io_ex.OV_RESET,1)
    time.sleep_ms(100)
    # end
    
    # Initialize OLED. Default settings: oled.init(port = 1,sda = 5,scl = 4,freq = 200000)
    display = oled.init(i2c1)
    
    # Display experiment information
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
        # Update the display
        display.refresh_gram()

        t = chr(ord(t) + 1)

        if t > '~':
        
            t = ' '
        
        time.sleep_ms(500)