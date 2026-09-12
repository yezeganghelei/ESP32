"""
 ******************************************************************************
 * @file     main.py
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    RGB screen touch experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Purpose of the experiment: Drive the touch function of a 4.3-inch RGB screen

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
 * 4,   TOUCH --> ESP32S3 IO
 *    IIC_SDA --> IO39
 *    IIC_CLK --> IO38
 *  TOUCH_INT --> IO40
 *  TOUCH_RST --> XL9555_P11

 * Experimental phenomenon
 * 1, At power-on, this experiment code first initializes the RGB screen and touch, then enters the
 *    relevant test. For a capacitive screen, it directly enters the handwriting test program. The
 *    capacitive touch screen supports the 4.3-inch screen module.
 * 2, The LED flashes, indicating that the program is running.

 * Things to note
 * none

 ******************************************************************************

 ******************************************************************************
"""

from machine import Pin,I2C
import atk_xl9555 as io_ex
import atk_ltdc as ltdc
import atk_touch as touch
import time

# Set the screen orientation: 0 is portrait; 1 is landscape
atk_dir = 1

if atk_dir == 1:
    LCD_WIDTH  = 800
    LCD_HEIGHT = 480
else:
    LCD_WIDTH  = 480
    LCD_HEIGHT = 800

"""
 * @brief Clear the screen and display "RST" in the upper right corner
 * @param None
 * @retval None
"""
def load_draw_dialog():
    
    display.clear(ltdc.WHITE)
    display.string(LCD_WIDTH - 30, 0, 200, 16, 16, "RST", ltdc.BLUE)

"""
 * @brief       Draw thick lines
 * @param       x1,y1: Starting point coordinates
 * @param       x2,y2: End point coordinates
 * @param       size : line thickness
 * @param       color: The color of the line
 * @retval      none
"""
def lcd_draw_bline(x1,y1,x2,y2,size,color):
    
    t = 0
    xerr = 0
    yerr = 0
    delta_x = 0
    delta_y = 0
    distance = 0
    incx = 0
    incy = 0
    row = 0
    col = 0

    delta_x = x2 - x1                       # Calculate coordinate increments
    delta_y = y2 - y1
    row = x1
    col = y1

    if delta_x > 0:
        incx = 1                            # Set single step direction  
    elif delta_x == 0:
        incx = 0                            # Vertical line
    else:
        incx = -1
        delta_x = -delta_x

    if delta_y > 0:
        incy = 1
    elif delta_y == 0:
        incy = 0                            # Horizontal line
    else:
        incy = -1
        delta_y = -delta_y

    if delta_x > delta_y:
        distance = delta_x;                 # Select base incremental axis
    else:
        distance = delta_y

    for t in range(0,distance + 1):         # Draw the line
        display.circle(row, col, size, color)   # Draw points
        xerr += delta_x
        yerr += delta_y

        if xerr > distance:
            xerr -= distance
            row += incx

        if yerr > distance:
            yerr -= distance
            col += incy

"""
 * @brief       Capacitive screen test
 * @param None
 * @retval None
"""
def ctp_test():
    
    t = 0
    i = 0
    lastpos = [[0, 0, 0, 0, 0, 0, 0, 0, 0, 0], [0, 0]]    # Last data

    while True:
        
        touch.scan(0)
        
        x_data = touch.get_x()
        y_data = touch.get_y()
        sta_data = touch.get_sta()
        
        x = (x_data[9] << 8) | x_data[8]
        y = (y_data[9] << 8) | y_data[8]
        sta = (sta_data[1] << 8) | sta_data[0]
        
        for t in range(0,5):
            
            if sta & (1 << t):
                
                if (x < LCD_WIDTH and y < LCD_HEIGHT):
                    
                    if lastpos[t][0] == 0xFFFF:
                        
                        lastpos[t][0] = x
                        lastpos[t][1] = y
                    
                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], x,y, 2, ltdc.RED);
                    lastpos[t][0] = x
                    lastpos[t][1] = y
                    
                    if (x > (LCD_WIDTH - 30) and y < 20):
                        
                        load_draw_dialog()
                else:
                    lastpos[t][0] = 0xFFFF
                
        time.sleep_ms(5)
        
        i += 1
        
        if (i % 20 == 0):
            
            led_state = led.value()
            led.value(not led_state)

"""
 * @brief program entry
 * @param None
 * @retval None
"""
if __name__ == '__main__':
   
    x = 0
    # Initialize LED and output high level
    led = Pin(1,Pin.OUT,value = 1)
    # IIC initialization
    i2c0 = I2C(0, scl = Pin(42), sda = Pin(41), freq = 400000)
    # XL9555 initialization
    xl9555 = io_ex.init(i2c0)

    # Initialize RGB
    display = ltdc.init(dir = atk_dir)
    # Turn on RGB screen backlight
    xl9555.write_bit(io_ex.LCD_BL,1)
    time.sleep_ms(100)
    
    # Reset touch chip
    xl9555.write_bit(io_ex.CT_RST,0)
    time.sleep_ms(10)
    xl9555.write_bit(io_ex.CT_RST,1)
    time.sleep_ms(10)
    
    # Initialize touch driver
    touch.init()
    
    # Clear the screen and display "RST" in the upper right corner
    load_draw_dialog()
    
    while True:
        
        ctp_test()