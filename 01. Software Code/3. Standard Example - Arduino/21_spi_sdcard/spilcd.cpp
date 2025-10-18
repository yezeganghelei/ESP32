/**
 ****************************************************************************************************
 * @file        spilcd.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SPILCD Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "spilcd.h"
#include "font.h"
#include <SPI.h>
#include "xl9555.h"

#define SPI_LCD_TYPE    1           /* SPIinterfaceScreentype（1：2.4inchSPILCD  0：1.3inchSPILCD） */  

/* The width and height definition of LCD */
#if SPI_LCD_TYPE                    /* 2.4-inch SPI_LCD screen */
uint16_t spilcd_width  = 240;       /* Screen width 240 (portrait screen) */
uint16_t spilcd_height = 320;       /* screen width 320(Vertical screen) */
#else
uint16_t spilcd_width  = 240;       /* Screen width 240 (portrait screen) */
uint16_t spilcd_height = 240;       /* Screen width 240 (portrait screen) */
#endif                              /* 1.3 inch SPI_LCD screen */

uint8_t spilcd_dir = 1;             /* Default landscape(1)、Vertical screen(0) */

#define USE_LCD_BUF    1            /* Used by defaultlcd_buf,uselcd_bufThe refresh speed will be greatly improved,Sacrifice space to increase speed,When there is not enough memory,Set0Just */
#if USE_LCD_BUF
    #if SPI_LCD_TYPE
    uint16_t lcd_buf[320 * 240];    /* Store a frame of image data */
    #else
    uint16_t lcd_buf[240 * 240];    /* Store a frame of image data */
    #endif
#endif

/* LCD brush color and background color */
uint32_t g_point_color = 0XF800;    /* Brush color */
uint32_t g_back_color  = 0XFFFF;    /* Background color */

static const int SPICLK = 80000000; /* SPI communication rate (screen display is abnormal, lower SPICLK) */

SPIClass* spi_lcd = NULL;           /* Define an uninitialized pointer to an SPI object */

/**
 * @brief Write commands to LCD
 * @param cmd: command
 * @retval None
 */
static void lcd_write_cmd(uint8_t cmd)
{
    LCD_WR(0);
    spi_lcd->transfer(cmd);
}

/**
 * @brief       PastLCDWrite data
 * @param       data:data
 * @retval      none
 */
static void lcd_write_data(uint8_t data)
{
    LCD_WR(1);
    spi_lcd->transfer(data);
}

/**
 * @brief	      PastLCDWrite the specified amount of data
 * @param       data:dataofstartaddress
 * @param       size:Send data size
 * @return      none
 */
static void lcd_write_bytes(uint8_t *data, uint32_t size)
{
    LCD_WR(1);
    spi_lcd->transfer(data, size);
}

/**
 * @brief PastLCD writes Pixel data
 * @param data:Pixel data
 * @retval none
 */
static void lcd_write_pixeldata(uint16_t data)
{
    LCD_WR(1);
    spi_lcd->transfer16(data);
}

/**
 * @brief set upLCDRow and queue address
 * @param xs: Column starting address
 * ys: Line start address
 * xe: Column end address
 * ye: Row Finish address
 * @retval none
 */
void lcd_set_address(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye) 
{
    lcd_write_cmd(0x2A);
    lcd_write_data((uint8_t)(xs >> 8) & 0xFF);
    lcd_write_data((uint8_t)xs & 0xFF);
    lcd_write_data((uint8_t)(xe >> 8) & 0xFF);
    lcd_write_data((uint8_t)xe & 0xFF);
    lcd_write_cmd(0x2B);
    lcd_write_data((uint8_t)(ys >> 8) & 0xFF);
    lcd_write_data((uint8_t)ys & 0xFF);
    lcd_write_data((uint8_t)(ye >> 8) & 0xFF);
    lcd_write_data((uint8_t)ye & 0xFF);
    lcd_write_cmd(0x2C);
}

/**
 * @brief       set upLCDofAutomatic scan direction
 * @note        generallyset upforL2R_U2DJust,ifset upFor other scanning methods,It may cause abnormal display.
 * @param       dir:0~7,represent8One direction(For specific definitions, seelcd.h)
 * @retval      none
 */
void lcd_scan_dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;

    if (spilcd_dir == 1)  /* Horizontal screen，Need to change the scanning direction！ */
    {
        switch (dir)      /* direction change */
        {
            case 0:
                dir = 5;
                break;

            case 1:
                dir = 7;
                break;

            case 2:
                dir = 4;
                break;

            case 3:
                dir = 6;
                break;

            case 4:
                dir = 1;
                break;

            case 5:
                dir = 0;
                break;

            case 6:
                dir = 3;
                break;

            case 7:
                dir = 2;
                break;
        }
    }

    /* Set the value of bit 5,6,7 of the 0x36 register bit according to the scan method */
    switch (dir)
    {
        case L2R_U2D:/* from left to right,from top to bottom */
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;

        case L2R_D2U:/* from left to right,from bottom to top */
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;

        case R2L_U2D:/* right to left,from top to bottom */
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;

        case R2L_D2U:/* right to left,from bottom to top */
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;

        case U2D_L2R:/* From top to bottom, from left to right */
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;

        case U2D_R2L:/* From top to bottom, from right to left */
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;

        case D2U_L2R:/* from bottom to top,from left to right */
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;

        case D2U_R2L:/* from bottom to top,right to left */
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
    }

    dirreg = 0x36;

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3)); 

    LCD_CS(0);  
    lcd_write_cmd(dirreg);
    lcd_write_data(regval);

    if (regval & 0x20)
    {
        if (spilcd_width < spilcd_height)   /* Exchange X, Y */
        {
            temp = spilcd_width;
            spilcd_width = spilcd_height;
            spilcd_height = temp;
        }
    }
    else
    {
        if (spilcd_width > spilcd_height)   /* Exchange X, Y */
        {
            temp = spilcd_width;
            spilcd_width = spilcd_height;
            spilcd_height = temp;
        }
    }

    lcd_set_address(0, 0, spilcd_width - 1, spilcd_height - 1);

    LCD_CS(1);                    
    spi_lcd->endTransaction();
}

/**
 * @brief       set upLCDShow direction
 * @param       dir:0,Vertical screen; 1,Horizontal screen
 * @retval      none
 */
void lcd_display_dir(uint8_t dir)
{
    spilcd_dir = dir;

    if (SPI_LCD_TYPE)   /* 2.4inchScreen needswantdo processing，1.3The screen does not need to be processed */
    {
        if (dir == 0)   /* Vertical screen */
        {
            spilcd_width = 240;
            spilcd_height = 320;   
        }
        else            /* Horizontal screen */
        {
            spilcd_width = 320; 
            spilcd_height = 240; 
        }
    }

    lcd_scan_dir(DFT_SCAN_DIR); 
}

/**
 * @brief       clear screenfunction
 * @param       color: wantclear screenofcolor
 * @retval      none
 */
void lcd_clear(uint16_t color) 
{
    uint32_t index = 0;
    uint32_t totalpoint = spilcd_width * spilcd_height;

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));
    LCD_CS(0);

    lcd_set_address(0, 0, spilcd_width - 1 ,spilcd_height - 1);

    LCD_WR(1);

#if USE_LCD_BUF 
    uint16_t color_tmp = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);   /* Need to convert the color value */
    for (uint32_t i = 0; i < spilcd_width * spilcd_height; i++)               /* rightlcd_bufFill in data */
    {
      lcd_buf[i] = color_tmp;
    }
    spi_lcd->transfer(lcd_buf, spilcd_width * spilcd_height * 2);             /* Send a frame of image data(unit:byte) */
#else
    for (index = 0; index < totalpoint; index++)
    {
        spi_lcd->transfer16(color);
    }
#endif

    LCD_CS(1);
    spi_lcd->endTransaction();
}

/**
* @brief       initializationspilcd
* @param       none
* @retval      none
*/
void lcd_init(void)
{
    /* initializationLCDThe pins needed for the screen */
    xl9555_io_config(SLCD_PWR, IO_SET_OUTPUT);
    xl9555_io_config(SLCD_RST, IO_SET_OUTPUT);
    pinMode(SLCD_WR_PIN, OUTPUT);

    /* LCDThe pins needed for the screenThe default state is high */
    xl9555_pin_set(SLCD_PWR, IO_SET_HIGH);
    xl9555_pin_set(SLCD_RST, IO_SET_HIGH);
    digitalWrite(SLCD_WR_PIN, HIGH);
  
    /* rightSPIMake configuration */
    spi_lcd = new SPIClass(HSPI);   /* createSPIClassSelect an instanceSPIbus(HSPI) */
    spi_lcd->begin(SLCD_SCK_PIN, SLCD_SDI_PIN, SLCD_SDA_PIN, SLCD_CS_PIN);  /* set upSPIcommunication lines */
    pinMode(SLCD_CS_PIN, OUTPUT);   /* Set CS pin to output mode */

    /* hardware reset */
    LCD_RST(1);
    delay(10);
    LCD_RST(0);
    delay(10);
    LCD_RST(1);
    delay(120);

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));    /* Used inSPISettingsCustomize inofmatchSetEnterOKSPIbusinitialization */
    
    LCD_CS(0);                  /* Pull chip select line low,Select device */

    /* rightLCDofregisterMake configuration */
#if SPI_LCD_TYPE                /* right2.4inchLCDregisterEnterOKset up */
    lcd_write_cmd(0x11);        /* Sleep Out */
    delay(120);                 /* wait for power stability */

    lcd_write_cmd(0x3A);        /* 65k mode */
    lcd_write_data(0x05);

    lcd_write_cmd(0xC5);        /* VCOM */
    lcd_write_data(0x1A);

    lcd_write_cmd(0x36);        /* ScreenShow directionset up */
    lcd_write_data(0x00);

    /*-------------ST7789V Frame rate setting-----------*/
    lcd_write_cmd(0xB2);        /* Porch Setting */
    lcd_write_data(0x05);
    lcd_write_data(0x05);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    lcd_write_cmd(0xB7);        /* Gate Control */
    lcd_write_data(0x05);       /* 12.2v   -10.43v */

    /*--------------ST7789V Power setting---------------*/
    lcd_write_cmd(0xBB);        /* VCOM */
    lcd_write_data(0x3F);

    lcd_write_cmd(0xC0);        /* Power control */
    lcd_write_data(0x2c);

    lcd_write_cmd(0xC2);		    /* VDV and VRH Command Enable */
    lcd_write_data(0x01);

    lcd_write_cmd(0xC3);        /* VRH Set */
    lcd_write_data(0x0F);       /* 4.3+( vcom+vcom offset+vdv) */

    lcd_write_cmd(0xC4);        /* VDV Set */
    lcd_write_data(0x20);       /* 0v */

    lcd_write_cmd(0xC6);        /* Frame Rate Control in Normal Mode */
    lcd_write_data(0X01);       /* 111Hz */

    lcd_write_cmd(0xD0);        /* Power Control 1 */
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xE8);        /* Power Control 1 */
    lcd_write_data(0x03);

    lcd_write_cmd(0xE9);        /* Equalize time control */
    lcd_write_data(0x09);
    lcd_write_data(0x09);
    lcd_write_data(0x08);

    /*---------------ST7789V gamma setting-------------*/
    lcd_write_cmd(0xE0);        /* Set Gamma */
    lcd_write_data(0xD0);
    lcd_write_data(0x05);
    lcd_write_data(0x09);
    lcd_write_data(0x09);
    lcd_write_data(0x08);
    lcd_write_data(0x14);
    lcd_write_data(0x28);
    lcd_write_data(0x33);
    lcd_write_data(0x3F);
    lcd_write_data(0x07);
    lcd_write_data(0x13);
    lcd_write_data(0x14);
    lcd_write_data(0x28);
    lcd_write_data(0x30);

    lcd_write_cmd(0XE1);        /* Set Gamma */
    lcd_write_data(0xD0);
    lcd_write_data(0x05);
    lcd_write_data(0x09);
    lcd_write_data(0x09);
    lcd_write_data(0x08);
    lcd_write_data(0x03);
    lcd_write_data(0x24);
    lcd_write_data(0x32);
    lcd_write_data(0x32);
    lcd_write_data(0x3B);
    lcd_write_data(0x14);
    lcd_write_data(0x13);
    lcd_write_data(0x28);
    lcd_write_data(0x2F);

    lcd_write_cmd(0x21);        /* Reverse */
    lcd_write_cmd(0x29);        /* Turn on display */
#else                           /* right1.3inchLCDregisterEnterOKset up */
    lcd_write_cmd(0x11);        /* Sleep Out */
    delay(120);                 /* wait for power stability */

    lcd_write_cmd(0x36);        /* Memory Data Access Control */
    lcd_write_data(0x00);

    lcd_write_cmd(0x3A);        /* RGB 5-6-5-bit  */
    lcd_write_data(0x65);

    lcd_write_cmd(0xB2);        /* Porch Setting */
    lcd_write_data(0x0C);
    lcd_write_data(0x0C);
    lcd_write_data(0x00);
    lcd_write_data(0x33);
    lcd_write_data(0x33);

    lcd_write_cmd(0xB7);        /*  Gate Control */
    lcd_write_data(0x72);

    lcd_write_cmd(0xBB);        /* VCOM Setting */
    lcd_write_data(0x3D);

    lcd_write_cmd(0xC0);        /* LCM Control */
    lcd_write_data(0x2C);

    lcd_write_cmd(0xC2);        /* VDV and VRH Command Enable */
    lcd_write_data(0x01);

    lcd_write_cmd(0xC3);        /* VRH Set */
    lcd_write_data(0x19);

    lcd_write_cmd(0xC4);        /* VDV Set */
    lcd_write_data(0x20);

    lcd_write_cmd(0xC6);        /* Frame Rate Control in Normal Mode */
    lcd_write_data(0x0F);

    lcd_write_cmd(0xD0);        /* Power Control 1 */
    lcd_write_data(0xA4);
    lcd_write_data(0xA1);

    lcd_write_cmd(0xE0);        /* Positive Voltage Gamma Control */
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0D);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2B);
    lcd_write_data(0x3F);
    lcd_write_data(0x54);
    lcd_write_data(0x4C);
    lcd_write_data(0x18);
    lcd_write_data(0x0D);
    lcd_write_data(0x0B);
    lcd_write_data(0x1F);
    lcd_write_data(0x23);

    lcd_write_cmd(0xE1);        /* Negative Voltage Gamma Control */
    lcd_write_data(0xD0);
    lcd_write_data(0x04);
    lcd_write_data(0x0C);
    lcd_write_data(0x11);
    lcd_write_data(0x13);
    lcd_write_data(0x2C);
    lcd_write_data(0x3F);
    lcd_write_data(0x44);
    lcd_write_data(0x51);
    lcd_write_data(0x2F);
    lcd_write_data(0x1F);
    lcd_write_data(0x1F);
    lcd_write_data(0x20);
    lcd_write_data(0x23);

    lcd_write_cmd(0x21);        /* Display Inversion On */
    lcd_write_cmd(0x29);
#endif
    LCD_CS(1);                  /* Pull up chip select line,Uncheck */  
    spi_lcd->endTransaction();  /* FinishSPItransmission */

    lcd_display_dir(1);         /* Default isHorizontal screen */
    lcd_display_on();           /* turn onLCDbacklight */
    lcd_clear(WHITE);           /* clear screen */
}

/**
 * @brief       turn onLCDbacklight
 * @param       none
 * @retval      none
 */
void lcd_display_on(void)
{
    LCD_PWR(1);
}

/**
 * @brief       closureLCDbacklight
 * @param       none
 * @retval      none
 */
void lcd_display_off(void)
{
    LCD_PWR(0);
}

/**
 * @brief       LCDArea Filling
 * @param       xs   : Regional startXcoordinate
 *              ys   : Regional startYcoordinate
 *              xe   : Regional terminationXcoordinate
 *              ye   : Regional terminationYcoordinate
 *              color: Area Fillingcolor
 * @retval      none
 */
void lcd_fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
    uint32_t area_size;
    uint16_t buf_index;
    
    area_size = (xe - xs + 1) * (ye - ys + 1) * sizeof(uint16_t);   /* Count the number of bytes in a region */

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));
    LCD_CS(0);

    lcd_set_address(xs, ys, xe, ye);
    LCD_WR(1);
    
#if USE_LCD_BUF
    uint16_t color_tmp = ((color & 0x00FF) << 8) | ((color & 0xFF00) >> 8);
    for (uint32_t i = 0; i < area_size / sizeof(uint16_t); i++)
    {
        lcd_buf[i] = color_tmp;
    }
    spi_lcd->transfer(lcd_buf, area_size);
#else
    for (buf_index = 0; buf_index < area_size / sizeof(uint16_t); buf_index++)
    {
        spi_lcd->transfer16(color);
    }
#endif

    LCD_CS(1);
    spi_lcd->endTransaction();
}

/**
 * @brief       LCDDraw dots
 * @param       x    : treatDraw dotsofXcoordinate
 *              y    : treatDraw dotsofYcoordinate
 *              color: treatDraw dotsofcolor
 * @retval      none
 */
void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));
    LCD_CS(0);

    lcd_set_address(x, y, x, y);

    lcd_write_pixeldata(color);

    LCD_CS(1);
    spi_lcd->endTransaction();
}

/**
 * @brief       LCDdraw line segment
 * @param       x1   : treatdraw line segmentendpoint1ofXcoordinate
 *              y1   : treatdraw line segmentendpoint1ofYcoordinate
 *              x2   : treatdraw line segmentendpoint2ofXcoordinate
 *              y2   : treatdraw line segmentendpoint2ofYcoordinate
 *              color: The color of the line segment to be drawn
 * @retval      none
 */
void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t x_delta;
    uint16_t y_delta;
    int16_t x_sign;
    int16_t y_sign;
    int16_t error;
    int16_t error2;
    
    x_delta = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    y_delta = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    x_sign = (x1 < x2) ? 1 : -1;
    y_sign = (y1 < y2) ? 1 : -1;
    error = x_delta - y_delta;
    
    lcd_draw_point(x2, y2, color);
    
    while ((x1 != x2) || (y1 != y2))
    {
        lcd_draw_point(x1, y1, color);
        
        error2 = error << 1;
        if (error2 > -y_delta)
        {
            error -= y_delta;
            x1 += x_sign;
        }
      
        if (error2 < x_delta)
        {
            error += x_delta;
            y1 += y_sign;
        }
    }
}

/**
 * @brief       draw horizontal line
 * @param       x,y   : Starting point coordinates
 * @param       len   : Line length
 * @param       color : rectangleofcolor
 * @retval      none
 */
void lcd_draw_hline(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
    if ((len == 0) || (x > spilcd_width) || (y > spilcd_height))
    {
        return;
    }

    lcd_fill(x, y, x + len - 1, y, color);
}

/**
 * @brief       LCDDraw a rectangular frame
 * @param       x1   : Endpoint of the rectangular frame to be drawn1ofXcoordinate
 *              y1   : Endpoint of the rectangular frame to be drawn1ofYcoordinate
 *              x2   : Endpoint of the rectangular frame to be drawn2ofXcoordinate
 *              y2   : Endpoint of the rectangular frame to be drawn2ofYcoordinate
 *              color: treatDraw a rectangular frameofcolor
 * @retval      none
 */
void lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}

/**
 * @brief       LCDDraw a circular frame
 * @param       x    : The origin of the circular frame to be drawnXcoordinate
 *              y    : The origin of the circular frame to be drawnYcoordinate
 *              r    : The radius of the circular frame to be drawn
 *              color: The color of the circular frame to be drawn
 * @retval      none
 */
void lcd_draw_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    int32_t x_t;
    int32_t y_t;
    int32_t error;
    int32_t error2;
    
    x_t = -r;
    y_t = 0;
    error = 2 - 2 * r;
    
    do 
    {
        lcd_draw_point(x - x_t, y + y_t, color);
        lcd_draw_point(x + x_t, y + y_t, color);
        lcd_draw_point(x + x_t, y - y_t, color);
        lcd_draw_point(x - x_t, y - y_t, color);
        
        error2 = error;
        if (error2 <= y_t)
        {
            y_t++;
            error = error + (y_t * 2 + 1);
            if ((-x_t == y_t) && (error2 <= x_t))
            {
                error2 = 0;
            }
        }
        
        if (error2 > x_t)
        {
            x_t++;
            error = error + (x_t * 2 + 1);
        }
    } while (x_t <= 0);
}

/**
 * @brief       filled circle
 * @param       x,y  : Circle center coordinates
 * @param       r    : radius
 * @param       color: Round color
 * @retval      none
 */
void lcd_fill_circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
    uint32_t i;
    uint32_t imax = ((uint32_t)r * 707) / 1000 + 1;
    uint32_t sqmax = (uint32_t)r * (uint32_t)r + (uint32_t)r / 2;
    uint32_t xr = r;

    lcd_draw_hline(x - r, y, 2 * r, color);

    for (i = 1; i <= imax; i++)
    {
      if ((i * i + xr * xr) > sqmax)
      {
          /* draw lines from outside */
          if (xr > imax)
          {
            lcd_draw_hline (x - i + 1, y + xr, 2 * (i - 1), color);
            lcd_draw_hline (x - i + 1, y - xr, 2 * (i - 1), color);
          }

          xr--;
      }

      /* draw lines from inside (center) */
      lcd_draw_hline(x - xr, y + i, 2 * xr, color);
      lcd_draw_hline(x - xr, y - i, 2 * xr, color);
    }
}

/**
 * @brief       LCDshow1characters
 * @param       x    : characters to be displayedXcoordinate
 *              y    : characters to be displayedYcoordinate
 *              ch   : treatshowcharacter
 *              font : The font of the characters to be displayed
 *              mode : Overlay method(1); NoOverlay method(0)
 *              color: characters to be displayedcolor
 * @retval      none
 */
void lcd_show_char(uint16_t x, uint16_t y, char ch, lcd_font_t font, uint8_t mode, uint16_t color)
{
    const uint8_t *ch_code;
    uint8_t ch_width;
    uint8_t ch_height;
    uint8_t ch_size;
    uint8_t ch_offset;
    uint8_t byte_index;
    uint8_t byte_code;
    uint8_t bit_index;
    uint8_t width_index = 0;
    uint8_t height_index = 0;
    
    ch_offset = ch - ' ';   /* Get the offset value (the ASCII font starts modulo from the space, so -' ' is the font of the corresponding character) */
    
    switch (font)   /* Get the height and width of the font */
    {
#if (FONT_12 != 0)
        case LCD_FONT_12:
        {
            ch_code = font_1206[ch_offset];
            ch_width = FONT_12_CHAR_WIDTH;
            ch_height = FONT_12_CHAR_HEIGHT;
            ch_size = FONT_12_CHAR_SIZE;
            break;
        }
#endif
#if (FONT_16 != 0)
        case LCD_FONT_16:
        {
            ch_code = font_1608[ch_offset];
            ch_width = FONT_16_CHAR_WIDTH;
            ch_height = FONT_16_CHAR_HEIGHT;
            ch_size = FONT_16_CHAR_SIZE;
            break;
        }
#endif
#if (FONT_24 != 0)
        case LCD_FONT_24:
        {
            ch_code = font_2412[ch_offset];
            ch_width = FONT_24_CHAR_WIDTH;
            ch_height = FONT_24_CHAR_HEIGHT;
            ch_size = FONT_24_CHAR_SIZE;
            break;
        }
#endif
#if (FONT_32 != 0)
        case LCD_FONT_32:
        {
            ch_code = font_3216[ch_offset];
            ch_width = FONT_32_CHAR_WIDTH;
            ch_height = FONT_32_CHAR_HEIGHT;
            ch_size = FONT_32_CHAR_SIZE;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    if ((x + ch_width > spilcd_width) || (y + ch_height > spilcd_height))
    {
        return;
    }
    
    for (byte_index = 0; byte_index < ch_size; byte_index++)
    {
        byte_code = ch_code[byte_index];                  /* Getcharacteroflatticedata */

        for (bit_index = 0; bit_index < 8; bit_index++)   /* 8 dots per byte */
        {
            if ((byte_code & 0x80) != 0)                  /* Valid points need to be displayed */
            {
                lcd_draw_point(x + width_index, y + height_index, color);           /* Draw dotscome out,To display this point */
            }
            else if (mode == 0)
            {
                lcd_draw_point(x + width_index, y + height_index, g_back_color);    /* paintingBackground color,Equivalent to thispointDon't show(Note that the background color is controlled by global variables) */
            }

            height_index++;

            if (height_index == ch_height)    /* One column has been displayed? */
            {
                height_index = 0;             /* yCoordinate reset */
                width_index++;                /* xcoordinate increment */
                break;
            }

            byte_code <<= 1;                  /* Shift, to get the status of the next bit */
        }
    }
}

/**
 * @brief LCD display string
 * @note will automatically wrap and pages
 * @param x : X coordinate of the string to be displayed
 * y : The Y coordinate of the string to be displayed
 * str: string to be displayed
 * font: The font of the string to be displayed
 * color: The color of the string to be displayed
 * @retval None
 */
void lcd_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, lcd_font_t font, char *str,  uint16_t color)
{
    uint8_t x0 = x;
    uint8_t ch_width;
    uint8_t ch_height;
    
    width += x;
    height += y;

    switch (font)   /* Get the height and width of the font */
    {
#if (FONT_12 != 0)
        case LCD_FONT_12:
        {
            ch_width = FONT_12_CHAR_WIDTH;
            ch_height = FONT_12_CHAR_HEIGHT;
            break;
        }
#endif
#if (FONT_16 != 0)
        case LCD_FONT_16:
        {
            ch_width = FONT_16_CHAR_WIDTH;
            ch_height = FONT_16_CHAR_HEIGHT;
            break;
        }
#endif
#if (FONT_24 != 0)
        case LCD_FONT_24:
        {
            ch_width = FONT_24_CHAR_WIDTH;
            ch_height = FONT_24_CHAR_HEIGHT;
            break;
        }
#endif
#if (FONT_32 != 0)
        case LCD_FONT_32:
        {
            ch_width = FONT_32_CHAR_WIDTH;
            ch_height = FONT_32_CHAR_HEIGHT;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
    
    while ((*str >= ' ') && (*str <= '~'))   /* Determine whether it is an illegal character! */
    {
        if (x >= width)
        {
            x = x0;
            y += ch_height;
        }
        
        if (y >= height)
        {
            break;
        }
      
      lcd_show_char(x, y, *str, font, 0, color);
      
      x += ch_width;
      str++;
    }
}

/**
 * @brief square function, x^y
 * @param x: base
 * y: exponent
 * @retval x^y
 */
static uint32_t lcd_pow(uint8_t x, uint8_t y)
{
    uint8_t loop;
    uint32_t res = 1;
    
    for (loop = 0; loop < y; loop++)
    {
        res *= x;
    }
    
    return res;
}

/**
 * @brief       LCDShow numbers，Controllable display high bit0
 * @param       x    : treatShow numbersofXcoordinate
 *              y    : treatShow numbersofYcoordinate
 *              num  : treatShow numbers
 *              len  : The number of digits to be displayed
 *              mode : NUM_SHOW_NOZERO: Number high0Don't show
 *                     NUM_SHOW_ZERO  : Number high0show
 *              font : Fonts to be displayed
 *              color: The color of the number to be displayed
 * @retval      none
 */
void lcd_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len,  lcd_font_t font, num_mode_t mode, uint16_t color)
{
    uint8_t ch_width;
    uint8_t len_index;
    uint8_t num_index;
    uint8_t first_nozero = 0;
    char pad;
  
    switch (font)
    {
#if (FONT_12 != 0)
        case LCD_FONT_12:
        {
            ch_width = FONT_12_CHAR_WIDTH;
            break;
        }
#endif
#if (FONT_16 != 0)
        case LCD_FONT_16:
        {
            ch_width = FONT_16_CHAR_WIDTH;
            break;
        }
#endif
#if (FONT_24 != 0)
        case LCD_FONT_24:
        {
            ch_width = FONT_24_CHAR_WIDTH;
            break;
        }
#endif
#if (FONT_32 != 0)
        case LCD_FONT_32:
        {
            ch_width = FONT_32_CHAR_WIDTH;
            break;
        }
#endif
        default:
        {
            return;
        }
    }
  
    switch (mode)
    {
        case NUM_SHOW_NOZERO:
        {
            pad = ' ';
            break;
        }
        case NUM_SHOW_ZERO:
        {
            pad = '0';
            break;
        }
        default:
        {
            return;
        }
    }
  
    for (len_index = 0; len_index < len; len_index++)                 /* Cycle by total number of displayed digits */
    {
        num_index = (num / lcd_pow(10, len - len_index - 1)) % 10;    /* Get the number of the corresponding bit */
        if ((first_nozero == 0) && (len_index < (len - 1)))           /* Display is not enabled and there are still bits to be displayed */
        {
            if (num_index == 0)
            {
                lcd_show_char(x + ch_width * len_index, y, pad, font, mode & 0x01, color);   /* The high bits need to be filled with 0 */
                continue;
            }
            else
            {
                first_nozero = 1;   /* Enable display */
            }
        }
        
        lcd_show_char(x + ch_width * len_index, y, num_index + '0', font, mode & 0x01, color);
    }
}

/**
 * @brief       LCDShow numbers，Don't showhigh position0
 * @param       x    : treatShow numbersofXcoordinate
 *              y    : treatShow numbersofYcoordinate
 *              num  : treatShow numbers
 *              len  : The number of digits to be displayed
 *              font : Fonts to be displayed
 *              color: The color of the number to be displayed
 * @retval      none
 */
void lcd_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, lcd_font_t font, uint16_t color)
{
    lcd_show_xnum(x, y, num, len, font,  NUM_SHOW_NOZERO, color);
}

/**
 * @brief LCDPicture display
 * @note Picture taking mode: horizontal scan、RGB565、High in front
 * @param x: Picture to be displayedXcoordinate
 *y: Picture to be displayedYcoordinate
 * width : The width of the image to be displayed
 * height: Picture to be displayed height
 * pic : The first address of the image array to be displayed
 * @retval none
 */
void lcd_show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic)
{
    if ((x + width > spilcd_width) || (y + height > spilcd_height))
    {
        return;
    }

    spi_lcd->beginTransaction(SPISettings(SPICLK, MSBFIRST, SPI_MODE3));                               
    LCD_CS(0);

    lcd_set_address(x, y, x + width - 1, y + height - 1);

    lcd_write_bytes(pic, width * height * sizeof(uint16_t));

    LCD_CS(1);
    spi_lcd->endTransaction();
}