/**
 ****************************************************************************************************
 * @file        lcd.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       SPI LCD(MCUscreen) Driver code
 *              Support driversICModels include:ILI9341wait

 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __LCD_H__
#define __LCD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "xl9555.h"
#include "spi.h"

/* Pin definition */
#define LCD_NUM_WR      GPIO_NUM_40
#define LCD_NUM_CS      GPIO_NUM_21

/* IOoperate */
#define LCD_WR(x)       do{ x ? \
                            (gpio_set_level(LCD_NUM_WR, 1)):    \
                            (gpio_set_level(LCD_NUM_WR, 0));    \
                        }while(0)

#define LCD_CS(x)       do{ x ? \
                            (gpio_set_level(LCD_NUM_CS, 1)):    \
                            (gpio_set_level(LCD_NUM_CS, 0));    \
                        }while(0)

#define LCD_PWR(x)       do{ x ? \
                            (xl9555_pin_write(SLCD_PWR_IO, 1)): \
                            (xl9555_pin_write(SLCD_PWR_IO, 0)); \
                        }while(0)

#define LCD_RST(x)       do{ x ? \
                            (xl9555_pin_write(SLCD_RST_IO, 1)): \
                            (xl9555_pin_write(SLCD_RST_IO, 0)); \
                        }while(0)

/* Common color values */
#define WHITE           0xFFFF      /* White */
#define BLACK           0x0000      /* black */
#define RED             0xF800      /* red */
#define GREEN           0x07E0      /* green */
#define BLUE            0x001F      /* blue */ 
#define MAGENTA         0XF81F      /* Tastered/Purple-red = BLUE + RED */
#define YELLOW          0XFFE0      /* Yellow = GREEN + RED */
#define CYAN            0X07FF      /* blue = GREEN + BLUE */  

/* Very color */
#define BROWN           0XBC40      /* brown */
#define BRRED           0XFC07      /* brown */
#define GRAY            0X8430      /* grey */ 
#define DARKBLUE        0X01CF      /* Dark blue */
#define LIGHTBLUE       0X7D7C      /* light blue */ 
#define GRAYBLUE        0X5458      /* Gray blue */ 
#define LIGHTGREEN      0X841F      /* light green */  
#define LGRAY           0XC618      /* Light grey(PANNEL),Form background color */ 
#define LGRAYBLUE       0XA651      /* Light gray-blue(Intermediate layer color) */ 
#define LBBLUE          0X2B12      /* Light brown blue(Select the reverse color of the entry) */ 

/* Scan direction definition */
#define L2R_U2D         0           /* From left to right,From top to bottom */
#define L2R_D2U         1           /* From left to right,From bottom to top */
#define R2L_U2D         2           /* From right to left,From top to bottom */
#define R2L_D2U         3           /* From right to left,From bottom to top */
#define U2D_L2R         4           /* From top to bottom, from left to right */
#define U2D_R2L         5           /* From top to bottom, from right to left */
#define D2U_L2R         6           /* From bottom to top,From left to right */
#define D2U_R2L         7           /* From bottom to top,From right to left */

#define DFT_SCAN_DIR    L2R_U2D     /* Default scanning direction */

/* screen */
#define LCD_320X240     0
#define LCD_240X240     1

/* LCDInformation structure */
typedef struct _lcd_obj_t
{
    uint16_t        width;          /* width */
    uint16_t        height;         /* high */
    uint8_t         dir;            /* Horizontal or vertical screen control：0，Vertical screen；1，Horizontal screen。 */
    uint16_t        wramcmd;        /* Start writinggraminstruction */
    uint16_t        setxcmd;        /* set upx coordinates instruction */
    uint16_t        setycmd;        /* set upy coordinates instruction */
    uint16_t        wr;             /* Command/Data IO */
    uint16_t        cs;             /* Film selectionIO */
} lcd_obj_t;

/* LCDCache size settings，Please note when modifying this value！！！！Modifying these two values ​​may affect the following functions lcd_clear/lcd_fill/lcd_draw_line */
#define LCD_TOTAL_BUF_SIZE      (320 * 240 * 2)
#define LCD_BUF_SIZE            15360

/* Export related variables */
extern lcd_obj_t lcd_self;
extern uint8_t lcd_buf[LCD_TOTAL_BUF_SIZE];

/* Function declaration */
void lcd_init(void);                                                                                                    /* Initialize LCD */
void lcd_clear(uint16_t color);                                                                                         /* Screen clearing function */
void lcd_scan_dir(uint8_t dir);                                                                                         /* set upLCDAutomatic scanning direction */
void lcd_write_data(const uint8_t *data, int len);                                                                      /* senddataarriveLCD */
void lcd_write_data16(uint16_t data);                                                                                   /* send16-bit data to LCD */
void lcd_set_cursor(uint16_t xpos, uint16_t ypos);                                                                      /* Set the cursor position */
void lcd_set_window(uint16_t xstar, uint16_t ystar,uint16_t xend,uint16_t yend);                                        /* Set the window size */
void lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color);                                      /* Fill a single color in a specified area */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color);                     /* showlenNumbers */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color);      /* Extended displaylenNumbers */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color);   /* showString */
void lcd_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color);                             /* Draw a rectangle */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                                              /* Draw horizontal lines */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,uint16_t color);                                  /* Line drawing function(straight line、Slash) */
void lcd_draw_pixel(uint16_t x, uint16_t y, uint16_t color);                                                            /* Draw a pixel point */
void lcd_show_char(uint16_t x, uint16_t y, uint8_t chr, uint8_t size, uint8_t mode, uint16_t color);                    /* at specified locationshowOne character */

#endif