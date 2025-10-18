/**
 ******************************************************************************************************
 * @file spilcd.h
 * @author
 * @version V1.0
 * @date 2023-12-01
 * @brief SPILCD driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ******************************************************************************************************
 */

#ifndef __SPILCD_H
#define __SPILCD_H

#include "Arduino.h"

/* LCD width and height definition */
extern uint16_t spilcd_width;
extern uint16_t spilcd_height;
extern uint8_t  spilcd_dir;  

/* Define LCD enabled fonts */
#define FONT_12             1
#define FONT_16             1
#define FONT_24             1
#define FONT_32             1

/* Enabled by defaultfont size 12 */
#if ((FONT_12 == 0) && (FONT_16 == 0) && (FONT_24 == 0) && (FONT_32 == 0))
#undef FONT_12
#define FONT_12 1
#endif

/* LCD display font enumeration */
typedef enum
{
#if (FONT_12 != 0)
  LCD_FONT_12,             /* font size 12 */
#endif
#if (FONT_16 != 0)
  LCD_FONT_16,             /* 16size font */
#endif
#if (FONT_24 != 0)
  LCD_FONT_24,             /* Font No. 24 */
#endif
#if (FONT_32 != 0)
  LCD_FONT_32,             /* Font No. 32 */
#endif
} lcd_font_t;

/* LCDShow numbersMode enumeration */
typedef enum
{
  NUM_SHOW_NOZERO = 0x00,  /* High digit0Don't show */
  NUM_SHOW_ZERO,           /* High digit0show */
} num_mode_t;

/* pindefinition */
#define SLCD_CS_PIN       21   
#define SLCD_SDA_PIN      11 
#define SLCD_SCK_PIN      12
#define SLCD_SDI_PIN      -1
#define SLCD_WR_PIN       40

/* The following twoSPI_LCDNeed to useIOexistxl9555.h already has definition in
 * #define SLCD_PWR_PIN
 * #define SLCD_RST_PIN  
 */

/* Macro Functions */
#define LCD_PWR(x)        xl9555_pin_set(SLCD_PWR, x ? IO_SET_HIGH : IO_SET_LOW)
#define LCD_RST(x)        xl9555_pin_set(SLCD_RST, x ? IO_SET_HIGH : IO_SET_LOW)
#define LCD_WR(x)         digitalWrite(SLCD_WR_PIN, x)
#define LCD_CS(x)         digitalWrite(SLCD_CS_PIN, x)

/******************************************************************************************/
/* LCDScan direction and color definition */

/* Scan direction definition */
#define L2R_U2D         0           /* from left to right,from top to bottom */
#define L2R_D2U         1           /* from left to right,from bottom to top */
#define R2L_U2D         2           /* From right to left,from top to bottom */
#define R2L_D2U         3           /* From right to left,from bottom to top */

#define U2D_L2R         4           /* From top to bottom, from left to right */
#define U2D_R2L         5           /* From top to bottom, from right to left */
#define D2U_L2R         6           /* from bottom to top,from left to right */
#define D2U_R2L         7           /* from bottom to top,From right to left */

#define DFT_SCAN_DIR    L2R_U2D     /* Default scanning direction */

/* Brush color */
#define WHITE               0xFFFF
#define BLACK               0x0000
#define BLUE                0x001F  
#define BRED                0XF81F
#define GRED                0XFFE0
#define GBLUE               0X07FF
#define RED                 0xF800
#define MAGENTA             0xF81F
#define GREEN               0x07E0
#define CYAN                0x7FFF
#define YELLOW              0xFFE0
#define BROWN               0XBC40      /* brown */
#define BRRED               0XFC07      /* brown red */
#define GRAY                0X8430      /* grey */
#define DARKBLUE            0X01CF      /* dark blue */
#define LIGHTBLUE           0X7D7C      /* Light blue */
#define GRAYBLUE            0X5458      /* gray blue */
/* The above three colors are the colors of PANEL  */
#define LIGHTGREEN          0X841F      /* light green */
#define LGRAY               0XC618      /* Light gray(PANNEL),Form background color */

#define LGRAYBLUE           0XA651      /* Light gray blue (middle layer color) */
#define LBBLUE              0X2B12      /* light brown blue(Select the reverse color of the entry) */

/* Function declaration */
/* lcd driver function */
void lcd_init(void);                                                                      /* lcd initialization function */
void lcd_set_address(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);                 /* set uplcdRow and column address */
void lcd_clear(uint16_t color);                                                           /* lcd screen clearing function */
void lcd_display_on(void);                                                                /* OpenlcdBacklight */
void lcd_display_off(void);                                                               /* closurelcdBacklight */
void lcd_fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);        /* lcdArea Filling */
void lcd_display_dir(uint8_t dir);
void lcd_scan_dir(uint8_t dir);

/* lcdFunctional Function */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color);                                  /* lcddraw dots */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);       /* lcddraw line segment */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color);                    /* lcdDraw horizontal lines */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);  /* LCD drawing rectangular frame */
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                     /* lcd drawing circular frame */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);                     /* Fill the solid circle */

void lcd_show_char(uint16_t x, uint16_t y, char ch, lcd_font_t font, uint8_t mode, uint16_t color);                         /* lcd displays 1 character */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_font_t font, char *str,  uint16_t color); /* lcd display string */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len,  lcd_font_t font, num_mode_t mode, uint16_t color);   /* lcdShow numbers，Can control the display high position0 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, lcd_font_t font, uint16_t color);                      /* lcdShow numbers，Don't showhigh position0 */
void lcd_show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic);                                   /* lcdImage display */

#endif