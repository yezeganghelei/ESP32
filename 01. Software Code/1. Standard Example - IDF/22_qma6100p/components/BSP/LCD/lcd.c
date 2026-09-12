/**
 ****************************************************************************************************
* @file        lcd.c
* @author      
* @version     V1.0
* @date        2023-08-26
* @brief       SPI LCD (MCU screen) driver code
*              Supported driver IC models include: ILI9341, etc.

* @license     Copyright (c) 2020-2032, 
****************************************************************************************************
* @attention

****************************************************************************************************
*/

#define __LCD_VERSION__  "1.0"

#include "lcd.h"
#include "lcdfont.h"

#define SPI_LCD_TYPE    1           /* SPI interface screen type (1: 2.4-inch SPI LCD, 0: 1.3-inch SPI LCD) */  

spi_device_handle_t MY_LCD_Handle;
uint8_t lcd_buf[LCD_TOTAL_BUF_SIZE];
lcd_obj_t lcd_self;

/* LCD initialization command/parameter set, stored in this structure  */
typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; /* There is no data in the data; bit 7 = delay after setting; 0xFF = cmds end */
} lcd_init_cmd_t;

/**
 * @brief Send commands to LCD, blocking and waiting for transmission to complete using polling (because the amount of data transmission is very small, processing in polling can improve the speed. The overhead of using interrupt methods exceeds that of polling methods)
 * @param cmd: 8-bit command to send
 * @retval None
 */
void lcd_write_cmd(const uint8_t cmd)
{
    LCD_WR(0);
    spi2_write_cmd(MY_LCD_Handle, cmd);
}

/**
 * @brief       Send data to the LCD using polling to block and wait for the transfer to complete (the amount of data is very small, so polling is faster; the overhead of interrupts exceeds that of polling)
 * @param       data: 8-bit data to send
 * @retval      none
 */
void lcd_write_data(const uint8_t *data, int len)
{
    LCD_WR(1);
    spi2_write_data(MY_LCD_Handle, data, len);
}

/**
 * @brief sends data to LCD, blocking and waiting for transmission to complete using polling (because the amount of data transmission is very small, processing in polling can improve the speed. The overhead of using interrupt mode exceeds that of polling mode)
 * @param data: 16-bit data to send
 * @retval None
 */
void lcd_write_data16(uint16_t data)
{
    uint8_t dataBuf[2] = {0,0};
    dataBuf[0] = data >> 8;
    dataBuf[1] = data & 0xFF;
    LCD_WR(1);
    spi2_write_data(MY_LCD_Handle, dataBuf,2);
}

/**
 * @brief Set window size
 * @param xstar: upper left corner x-axis
 * @param ystar: upper left corner y-axis
 * @param xend: x-axis at the bottom right corner
 * @param yend: lower right corner y-axis
 * @retval None
 */
void lcd_set_window(uint16_t xstar, uint16_t ystar,uint16_t xend,uint16_t yend)
{	
    uint8_t databuf[4] = {0,0,0,0};
    databuf[0] = xstar >> 8;
    databuf[1] = 0xFF & xstar;
    databuf[2] = xend >> 8;
    databuf[3] = 0xFF & xend;
    lcd_write_cmd(lcd_self.setxcmd);
    lcd_write_data(databuf,4);

    databuf[0] = ystar >> 8;
    databuf[1] = 0xFF & ystar;
    databuf[2] = yend >> 8;
    databuf[3] = 0xFF & yend;
    lcd_write_cmd(lcd_self.setycmd);
    lcd_write_data(databuf,4);

    lcd_write_cmd(lcd_self.wramcmd);    /* Start writing to GRAM */
}   

/**
 * @brief       Clear the LCD screen with a single color
 * @param       color: Screen clear color
 * @retval      none
 */
void lcd_clear(uint16_t color)
{
    uint16_t i, j;
    uint8_t data[2] = {0};

    data[0] = color >> 8;
    data[1] = color;
    
    lcd_set_window(0, 0, lcd_self.width - 1, lcd_self.height - 1);

    for(j = 0; j < LCD_BUF_SIZE / 2; j++)
    {
        lcd_buf[j * 2] =  data[0];
        lcd_buf[j * 2 + 1] =  data[1];
    }

    for(i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_BUF_SIZE); i++)
    {
        lcd_write_data(lcd_buf, LCD_BUF_SIZE);
    }
}

/**
 * @brief filling single individual color in the specified area
 * @param (sx,sy),(ex,ey):Fill the diagnostic coordinates of rectangles,The area size is:(ex - sx + 1) * (ey - sy + 1)
 * @param color:Colors to be filled(32bit color,Convenient and compatibleLTDC)
 * @retval none
 */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i;
    uint16_t j;
    uint16_t width;
    uint16_t height;

    width = ex - sx + 1;
    height = ey - sy + 1;
    lcd_set_window(sx, sy, ex, ey);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            lcd_write_data16(color);
        }
    }
    lcd_set_window(sx, sy, ex, ey);
}

/**
 * @brief       Set the cursor position
 * @param       Xpos: Top-left corner X axis
 * @param       Ypos: Top-left corner Y axis
 * @retval      none
 */
void lcd_set_cursor(uint16_t xpos, uint16_t ypos)
{
    lcd_set_window(xpos,ypos,xpos,ypos);	
} 

/**
 * @brief       Set the LCD auto-scan direction (ignored for RGB screens)
 * @param       dir: 0~7, one of 8 directions (see lcd.h for definitions)
 * @retval      none
 */
void lcd_scan_dir(uint8_t dir)
{
    uint8_t regval = 0;
    uint8_t dirreg = 0;
    uint16_t temp;

    /* In landscape mode, the scan direction is not changed for ILI9341 but is changed for other ICs; in portrait mode, ILI9341 changes direction while other ICs do not */
    if (lcd_self.dir == 1)
    {
        dir = 5;
    }

    /* Set bits 5, 6, 7 of register 0X36/0X3600 according to the scan direction */
    switch (dir)
    {
        case L2R_U2D:                           /* From left to right,From top to bottom */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:                           /* Left to right, bottom to top */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:                           /* From right to left,From top to bottom */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:                           /* Right to left, bottom to top */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:                           /* Top to bottom, left to right */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:                           /* Top to bottom, right to left */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:                           /* Bottom to top, left to right */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:                           /* Bottom to top, right to left */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0x36;                              /* For most driver ICs, controlled by register 0X36 */
    
    uint8_t date_send[1] = {regval};
    
    lcd_write_cmd(dirreg);
    lcd_write_data(date_send,1);
    
    if (regval & 0x20)
    {
        if (lcd_self.width < lcd_self.height)   /* Swap X and Y */
        {
            temp = lcd_self.width;
            lcd_self.width = lcd_self.height;
            lcd_self.height = temp;
        }
    }
    else
    {
        if (lcd_self.width > lcd_self.height)   /* Swap X and Y */
        {
            temp = lcd_self.width;
            lcd_self.width = lcd_self.height;
            lcd_self.height = temp;
        }
    }
    
    lcd_set_window(0, 0, lcd_self.width,lcd_self.height);
}

/**
 * @brief       Set the LCD display orientation
 * @param       dir:0, portrait; 1, landscape
 * @retval      none
 */
void lcd_display_dir(uint8_t dir)
{
    lcd_self.dir = dir;
    
    if (lcd_self.dir == 0)                  /* Portrait */
    {
        lcd_self.width      = 240;
        lcd_self.height     = 320;
        lcd_self.wramcmd    = 0X2C;
        lcd_self.setxcmd    = 0X2A;
        lcd_self.setycmd    = 0X2B;
    }
    else                                    /* Landscape */
    {
        lcd_self.width      = 320;          /* Default width */
        lcd_self.height     = 240;          /* Default height */
        lcd_self.wramcmd    = 0X2C;
        lcd_self.setxcmd    = 0X2A;
        lcd_self.setycmd    = 0X2B;
    }

    lcd_scan_dir(DFT_SCAN_DIR);             /* Default scanning direction */
}

/**
 * @brief Hardware reset
 * @param self_in: LCD structure
 * @retval None
 */
void lcd_hard_reset(void)
{
    /* Reset the display */
    LCD_RST(0);
    vTaskDelay(100);
    LCD_RST(1);
    vTaskDelay(100);
}

/**
 * @brief       Draw a single pixel
 * @param       self_in: LCD structure
 * @param       x: X-axis coordinate
 * @param       y: Y-axis coordinate
 * @param       color: Color value
 * @retval      none
 */
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_set_cursor(x, y);
    lcd_write_data16(color);
}

/**
 * @brief       Line drawing function (horizontal/vertical and diagonal lines)
 * @param       x1,y1   Starting point coordinates
 * @param       x2,y2   End point coordinates
 * @param       color Fill color
 * @retval      none
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t; 
    int xerr = 0, yerr = 0, delta_x, delta_y, distance; 
    
    int incx, incy, urow, ucol; 

    delta_x = x2 - x1;                      /* Calculate coordinate increment */
    delta_y = y2 - y1; 
    urow = x1; 
    ucol = y1; 
    
    if (delta_x > 0)
    {
        incx = 1;                           /* Set single step direction */
    }
    else if (delta_x == 0)
    {
        incx = 0;                           /* Vertical line */
    }
    else
    {
        incx =-1;
        delta_x =-delta_x;
    } 
    if(delta_y > 0)
    {
        incy = 1; 
    }
    else if(delta_y == 0)
    {
        incy = 0;                           /* Horizontal line */
    }
    else
    {
        incy =-1;
        delta_y=-delta_y;
    } 
    
    if( delta_x>delta_y)
    {
        distance = delta_x;                 /* Select the dominant increment axis */
    }
    else
    {
        distance = delta_y; 
    }
    
    for (t = 0;t <= distance + 1;t++ )      /* Line drawing output */
    {
        lcd_draw_pixel(urow,ucol,color);    /* Draw pixel */ 
        xerr += delta_x ; 
        yerr += delta_y ; 
        
        if(xerr>distance)
        { 
            xerr -= distance; 
            urow += incx; 
        } 
        
        if (yerr > distance)
        { 
            yerr -= distance; 
            ucol += incy; 
        } 
    } 
}

/**
 * @brief draw horizontal lines
 * @param x0,y0: Starting point coordinates
 * @param len : Line length
 * @param color: The color of the rectangle
 * @retval None
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > lcd_self.width) || (y > lcd_self.height))return;

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       Draw a rectangle
 * @param       x1,y1   Starting point coordinates
 * @param       x2,y2   End point coordinates
 * @param       color Fill color
 * @retval      none
 */
void lcd_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
    lcd_draw_line(x0, y0, x1, y0,color);
    lcd_draw_line(x0, y0, x0, y1,color);
    lcd_draw_line(x0, y1, x1, y1,color);
    lcd_draw_line(x1, y0, x1, y1,color);
}

/**
 * @brief       Draw a circle
 * @param       x0,y0   circle center coordinate
 * @param       r   Circle radius
 * @param       color Fill color
 * @retval      none
 */
void lcd_draw_circle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while (a <= b)
    {
        lcd_draw_pixel(x0 - b, y0 - a, color);
        lcd_draw_pixel(x0 + b, y0 - a, color);
        lcd_draw_pixel(x0 - a, y0 + b, color);
        lcd_draw_pixel(x0 - b, y0 - a, color);
        lcd_draw_pixel(x0 - a, y0 - b, color);
        lcd_draw_pixel(x0 + b, y0 + a, color);
        lcd_draw_pixel(x0 + a, y0 - b, color);
        lcd_draw_pixel(x0 + a, y0 + b, color);
        lcd_draw_pixel(x0 - b, y0 + a, color);
        a++;

        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }

        lcd_draw_pixel(x0 + a, y0 + b, color);
    }
}

/**
 * @brief       Display one character at the specified position
 * @param       x,y  : coordinate
 * @param       chr  : Characters to be displayed:" "--->"~"
 * @param       size : Font size 12/16/24/32
 * @param       mode : Overlay method(1); Non-overlapping method(0);
 * @param       color : Character color;
 * @retval      none
 */
void lcd_show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp = 0,t1 = 0, t = 0;
    uint8_t *pfont = 0;
    uint8_t csize = 0;                                      /* Number of bytes occupied by one character in the selected font */
    uint16_t colortemp = 0;
    uint8_t sta = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* Number of bytes occupied by one character in the selected font */
    chr = chr - ' ';                                        /* Get the offset value (the ASCII font library starts at space, so subtracting ' ' selects the corresponding character) */

    if ((x > (lcd_self.width - size / 2)) || (y > (lcd_self.height - size)))
    {
        return;
    }

    lcd_set_window(x, y, x + size / 2 - 1, y + size - 1);   /* (x,y,x+8-1,y+16-1) */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[chr];              /* Use 1206 font */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[chr];              /* Call 1608 font */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[chr];              /* Call 2412 font */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[chr];              /* Call 3216 font */
            break;

        default:
            return ;
    }

    if (size != 24)
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
        
        for (t = 0; t < csize; t++)
        {
            temp = pfont[t];                                /* Get the dot matrix data of characters */

            for (t1 = 0; t1 < 8; t1++)
            {
                    if (temp & 0x80)
                    {
                        colortemp = color;
                    }
                    else if (mode == 0)                     /* Invalid pixel, do not display */
                    {
                        colortemp = 0xFFFF;
                    }

                    lcd_write_data16(colortemp);
                    temp <<= 1;
            }
        }
    }
    else
    {
        csize = (size * 16) / 8;
        
        for (t = 0; t < csize; t++)
        {
            temp = asc2_2412[chr][t];

            if (t % 2 == 0)
            {
                sta = 8;
            }
            else
            {
                sta = 4;
            }

            for (t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80)
                {
                    colortemp = color;
                }
                else if (mode == 0)                         /* Invalid pixel, do not display */
                {
                    colortemp = 0xFFFF;
                }

                lcd_write_data16(colortemp);
                temp <<= 1;
            }
        }
    }
}

/**
 * @brief       m^n function
 * @param       m,n     Input parameters
 * @retval      m to the power of n
 */
uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

    return result;
}

/**
 * @brief Display len numbers
 * @param x,y : Start coordinates
 * @param num : Value (0 ~ 2^32)
 * @param len : Display the number of digits
 * @param size: Select font 12/16/24/32
 * @retval None
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                               /* Loop over the total number of digits */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                       /* Get the digit at the corresponding position */

        if (enshow == 0 && t < (len - 1))                                   /* Display is not yet enabled and there are more digits to show */
        {
            if (temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, ' ', size, 0, color);    /* Display space as placeholder */
                continue;                                                   /* Continue to next digit */
            }
            else
            {
                enshow = 1;                                                 /* Enable display */
            }

        }

        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, 0, color);     /* Show characters */
    }
}

/**
 * @brief       Extended number display (also shows leading zeros)
 * @param       x,y : Start coordinates
 * @param       num : Value(0 ~ 2^32)
 * @param       len : Display the number of digits
 * @param       size: Select a font 12/16/24/32
 * @param       mode: Display mode
 *              [7]: 0 = no fill, 1 = fill with 0.
 *              [6:1]: Reserved
 *              [0]: 0 = no overlay, 1 = overlay.
 * @param       color : Color of the numbers;
 * @retval      none
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                                           /* Loop over the total number of digits */
    {
        temp = (num / lcd_pow(10, len - t - 1)) % 10;                                   /* Get the digit at the corresponding position */

        if (enshow == 0 && t < (len - 1))                                               /* Display is not yet enabled and there are more digits to show */
        {
            if (temp == 0)
            {
                if (mode & 0X80)                                                        /* Leading positions should be filled with 0 */
                {
                    lcd_show_char(x + (size / 2)*t, y, '0', size, mode & 0X01, color);  /* Take up the place with 0 */
                }
                else
                {
                    lcd_show_char(x + (size / 2)*t, y, ' ', size, mode & 0X01, color);  /* Placeholder with spaces */
                }
                continue;
            }
            else
            {
                enshow = 1;                                                             /* Enable display */
            }
        }
        lcd_show_char(x + (size / 2)*t, y, temp + '0', size, mode & 0X01, color);
    }
}

/**
 * @brief       Display a string of characters
 * @param       x,y         : Start coordinates
 * @param       width,height: Area size
 * @param       size        : Select a font 12/16/24/32
 * @param       p           : Start address of the string
 * @retval      none
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* Check whether the character is valid */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)break;  /* Exit */

        lcd_show_char(x, y, *p, size, 0, color);
        x += size / 2;
        p++;
    }
}

/**
 * @brief       Turn on the LCD
 * @param       self_in: SPI control block
 * @retval      mp_const_none: Initialization succeeded
 */
void lcd_on(void)
{
    LCD_PWR(1);
    vTaskDelay(10);
}

/**
 * @brief       Turn off the LCD
 * @param       self_in: SPI control block
 * @retval      mp_const_none: Initialization succeeded
 */
void lcd_off(void)
{
    LCD_PWR(0);
    vTaskDelay(10);
}

/**
 * @brief LCD initialization
 * @param None
 * @retval None
 */
void lcd_init(void)
{
    int cmd = 0;
    esp_err_t ret = 0;
    
    lcd_self.dir = 0;
    lcd_self.wr = LCD_NUM_WR;                                       /* Configure the WR pin */
    lcd_self.cs = LCD_NUM_CS;                                       /* Configure the CS pin */
    
    gpio_config_t gpio_init_struct;

    /* SPI driver interface configuration */
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 60 * 1000 * 1000,                         /* SPI clock */
        .mode = 0,                                                  /* SPI mode 0 */
        .spics_io_num = lcd_self.cs,                                /* SPI device pin */
        .queue_size = 7,                                            /* Transaction queue size: 7 */
    };
    
    /* Add SPI bus device */
    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &MY_LCD_Handle);   /* Configure the SPI bus device */
    ESP_ERROR_CHECK(ret);

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;                       /* Configure output mode */
    gpio_init_struct.pin_bit_mask = 1ull << lcd_self.wr;            /* Configure pin bit mask */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* Disable pull-down */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;               /* Enable pull-up */
    gpio_config(&gpio_init_struct);                                 /* Pin configuration */

    lcd_hard_reset();                                               /* LCD hardware reset */

    /* Initialization code */
#if SPI_LCD_TYPE                                                    /* Set up the 2.4-inch LCD register */
    lcd_init_cmd_t ili_init_cmds[] =
    {
        {0x11, {0}, 0x80},
        {0x36, {0x00}, 1},
        {0x3A, {0x65}, 1},
        {0X21, {0}, 0x80},
        {0x29, {0}, 0x80},
        {0, {0}, 0xff},
    };

#else                                                               /* Otherwise it is treated as a 1.3-inch SPI LCD screen, which does not display inverted */
    lcd_init_cmd_t ili_init_cmds[] =
    {
        {0x11, {0}, 0x80},
        {0x36, {0x00}, 1},
        {0x3A, {0x65}, 1},
        {0xB2, {0x0C, 0x0C, 0x00, 0x33,0x33}, 5},
        {0xB7, {0x75}, 1},
        {0xBB, {0x1C}, 1},
        {0xC0, {0x2c}, 1},
        {0xC2, {0x01}, 1},
        {0xC3, {0x0F}, 1},
        {0xC4, {0x20}, 1},
        {0xC6, {0X01}, 1},
        {0xD0, {0xA4,0xA1}, 2},
        {0xE0, {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23}, 14},
        {0xE1, {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23}, 14},
        {0X21, {0}, 0x80},
        {0x29, {0}, 0x80},
        {0, {0}, 0xff},
    };
#endif

    /* Send all initialization register settings */
    while (ili_init_cmds[cmd].databytes != 0xff)
    {
        lcd_write_cmd(ili_init_cmds[cmd].cmd);
        lcd_write_data(ili_init_cmds[cmd].data, ili_init_cmds[cmd].databytes & 0x1F);
        
        if (ili_init_cmds[cmd].databytes & 0x80)
        {
            vTaskDelay(120);
        }
        
        cmd++;
    }

    lcd_display_dir(1);                                             /* Set screen orientation */
    LCD_PWR(1);
    lcd_clear(WHITE);                                               /* Clear the screen */
}