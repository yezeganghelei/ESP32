"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    RGBScreen experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment: How to drive a 4.3-inch RGB screen

 * Hardware resources and pin assignments: 
 * 1,     LED --> ESP32S3 IO
 *        LED --> IO1
 * 2,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> IO0(Jumper cap connection) 
 * 3,  RGBLCD --> ESP32S3 IO
 *     LCD_BL --> XL9555_P13
 *     LCD_DE --> IO4
 *  LCD_VSYNC --> NC
 *  LCD_HSYNC --> NC
 *   LCD_PCLK --> IO5
 *     LCD_R3 --> IO45
 *     LCD_R4 --> IO48
 *     LCD_R5 --> IO47
 *     LCD_R6 --> IO21
 *     LCD_R7 --> IO14
 *     LCD_G2 --> IO10
 *     LCD_G3 --> IO9
 *     LCD_G4 --> IO46
 *     LCD_G5 --> IO3
 *     LCD_G6 --> IO8
 *     LCD_G7 --> IO18
 *     LCD_B3 --> IO17
 *     LCD_B4 --> IO16
 *     LCD_B5 --> IO15
 *     LCD_B6 --> IO7
 *     LCD_B7 --> IO6

 * Experimental phenomenon
 * 1, After the program is downloaded successfully, the RGB screen displays the experiment information
 *    and refreshes the color.
 * 2, The LED flashes, indicating that the program is running.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,I2C
import atk_xl9555 as io_ex
import atk_ltdc as ltdc
import time

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
   
    x = 0
    # Initialize the LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC Initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
    xl9555 = io_ex.init(i2c0)
    
    # Initialize RGB
    display = ltdc.init(dir = 1)
    # Turn on the RGB screen backlight
    xl9555.write_bit(io_ex.LCD_BL,1)
    time.sleep_ms(100)
    
    while True:
        
        # Create a dictionary
        seasondict = {
        0: ltdc.BLACK,
        1: ltdc.BLUE,
        2: ltdc.RED,
        3: ltdc.GREEN,
        4: ltdc.CYAN,
        5: ltdc.MAGENTA,
        6: ltdc.YELLOW}

        # Refresh color
        display.clear(seasondict[x])
        # Display text
        display.string(0, 5, 240, 32, 32, "ESP32S3",ltdc.RED)
        display.string(0, 34, 240, 16, 16, "RGB Test",ltdc.RED)
        display.string(0, 50, 240, 16, 16, "ATOM@ALIENTEK",ltdc.RED)
        x += 1
        
        if x == 7:
            x = 0
        
        led_state = led.value()
        led.value(not led_state)
        time.sleep(1)