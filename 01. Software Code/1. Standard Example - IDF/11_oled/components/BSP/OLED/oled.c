/**
 ****************************************************************************************************
* @file        oled.c
* @author      
* @version     V1.0
* @date        2023-08-26
* @brief       OLEDDriver code
* @license     Copyright (c) 2020-2032, 
****************************************************************************************************
* @attention

****************************************************************************************************
*/

#define __OLED_VERSION__  "1.0"

#include "oled.h"
#include "oledfont.h"

i2c_obj_t oled_master;

/*OLEDVideo memory
    The storage format is as follows.
    [0]0 1 2 3 ... 127
    [1]0 1 2 3 ... 127
    [2]0 1 2 3 ... 127
    [3]0 1 2 3 ... 127
    [4]0 1 2 3 ... 127
    [5]0 1 2 3 ... 127
    [6]0 1 2 3 ... 127
    [7]0 1 2 3 ... 127
*/
uint8_t OLED_GRAM[128][8];

/**
 * @brief       oled IICWritedata
 * @param       data_wr：Sent data or commands
 * @param       size   ：senddataThe size of
 * @retval      0：Send successfully；No0value：Send failed
 */
esp_err_t oled_write(uint8_t* data_wr, size_t size)
{
    i2c_buf_t bufs = {
        .len = size,
        .buf = data_wr,
    };

    i2c_transfer(&oled_master, OLED_ADDR, 1, &bufs, I2C_FLAG_STOP);
    return ESP_OK;
}

/**
 * @brief oled Write command
 * @param tx_data: data
 * @param command: command value
 * @retval None
 */
void oled_write_Byte(unsigned char tx_data, unsigned char command)
{
    unsigned char data[2] = {command, tx_data};
    oled_write(data, sizeof(data));
}

/**
 * @brief       Update video memory toLCD
 * @param       none
 * @retval      none
 */
void oled_refresh_gram(void)
{
    uint8_t i, n;
    
    for (i = 0; i < 8; i++)
    {  
        oled_write_Byte(0xb0 + i, OLED_CMD);     /* Set page address (0~7) */
        oled_write_Byte(0x00, OLED_CMD);         /* Set display location—column low address */
        oled_write_Byte(0x10, OLED_CMD);         /* Set the display position—List the address */
        
        for (n = 0; n < 128; n++)
        {
            oled_write_Byte(OLED_GRAM[n][i], OLED_DATA);
        }
    }
}

/**
 * @brief       initializationOLED
 * @param       i2c_obj_t self: IncomingIICInitialization parameters，Used to determine whether it has been completedIICinitialization
 * @retval      none
 */
void oled_init(i2c_obj_t self)
{
    if (self.init_flag == ESP_FAIL)
    {
        iic_init(I2C_NUM_1);                                /* Initialize IIC */
    }

    oled_master = self;
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* Input and output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* Enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* Disabled pull-down */
    gpio_init_struct.pin_bit_mask = 1ULL << OLED_D2_PIN;    /* Set the bit mask of the pin */
    gpio_config(&gpio_init_struct);

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_OUTPUT;               /* Input and output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* Enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* Disabled pull-down */
    gpio_init_struct.pin_bit_mask = 1ULL << OLED_DC_PIN;    /* Set the bit mask of the pin */
    gpio_config(&gpio_init_struct);
    
    OLED_DC(0);
    
    /* ConfigurationResetPin Level */
    xl9555_pin_write(OV_RESET_IO, 1);

    /*Reset OLED*/
    OLED_RST(0);
    vTaskDelay(100);
    OLED_RST(1);
    vTaskDelay(100);

    /* initializationCode */
    oled_write_Byte(0xAE, OLED_CMD);    /* Close the display */
    oled_write_Byte(0xD5, OLED_CMD);    /* Set the clock frequency division factor,Oscillation frequency */
    oled_write_Byte(80, OLED_CMD);      /* [3:0],Dividing factor;[7:4],Oscillation frequency */
    oled_write_Byte(0xA8, OLED_CMD);    /* set upNumber of drive paths */
    oled_write_Byte(0X3F, OLED_CMD);    /* default0X3F(1/64) */
    oled_write_Byte(0xD3, OLED_CMD);    /* set upshowOffset */
    oled_write_Byte(0X00, OLED_CMD);    /* Default is0 */

    oled_write_Byte(0x40, OLED_CMD);    /* Set the display start line [5:0],Number of rows */

    oled_write_Byte(0x8D, OLED_CMD);    /* Charge pump settings */
    oled_write_Byte(0x14, OLED_CMD);    /* bit2, on/off */
    oled_write_Byte(0x20, OLED_CMD);    /* Set memory address mode */
    oled_write_Byte(0x02, OLED_CMD);    /* [1:0],00, column address mode; 01, row address mode; 10, page address mode; default 10; */
    oled_write_Byte(0xA1, OLED_CMD);    /* Segment redefinition settings, bit0:0,0->0;1,0->127; */
    oled_write_Byte(0xC0, OLED_CMD);    /* set upCOMScan direction;bit3:0,Normal mode;1,Redefine the pattern COM[N-1]->COM0;N:Number of drive paths */
    oled_write_Byte(0xDA, OLED_CMD);    /* set upCOMHardware pinsConfiguration */
    oled_write_Byte(0x12, OLED_CMD);    /* [5:4]Configuration */
            
    oled_write_Byte(0x81, OLED_CMD);    /* Contrast settings */
    oled_write_Byte(0xEF, OLED_CMD);    /* 1~255;default0X7F (Brightness settings,The bigger the brighter the) */
    oled_write_Byte(0xD9, OLED_CMD);    /* Set pre-charge cycle */
    oled_write_Byte(0xf1, OLED_CMD);    /* [3:0],PHASE 1;[7:4],PHASE 2; */
    oled_write_Byte(0xDB, OLED_CMD);    /* set upVCOMH Voltage multiplier */
    oled_write_Byte(0x30, OLED_CMD);    /* [6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc; */

    oled_write_Byte(0xA4, OLED_CMD);    /* GlobalshowOpen;bit0:1,Open;0,closure;(White screen/Black screen) */
    oled_write_Byte(0xA6, OLED_CMD);    /* Set display mode; bit0:1, reverse display; 0, normal display */
    oled_write_Byte(0xAF, OLED_CMD);    /* Openshow */
    /* Openoled */
    oled_on();
    oled_clear();
}

/**
 * @brief Open OLED
 * @param None
 * @retval None
 */
void oled_on(void)
{
    oled_write_Byte(0X8D, OLED_CMD);
    oled_write_Byte(0X14, OLED_CMD);
    oled_write_Byte(0XAF, OLED_CMD);
}

/**
 * @brief Close OLED
 * @param None
 * @retval None
 */
void oled_off(void)
{
    oled_write_Byte(0X8D, OLED_CMD);
    oled_write_Byte(0X10, OLED_CMD);
    oled_write_Byte(0XAE, OLED_CMD);
}

/**
 * @brief Clear screen
 * @param None
 * @retval None
 */
void oled_clear(void)
{
    uint8_t i,n;

    for (i = 0;i < 8;i++)
    {
        for (n = 0;n < 128;n++)
        {
            OLED_GRAM[n][i] = 0X00;
        }
    }

    oled_refresh_gram();    /* Update display */
}

/**
 * @brief OLED drawing points
 * @param x : 0~127
 * @param y : 0~63
 * @param dot: 1 Fill 0, clear
 * @retval None
 */ 
void oled_draw_point(uint8_t x, uint8_t y, uint8_t dot)
{
    uint8_t pos, bx, temp = 0;

    if (x > 127 || y > 63)
    {
        return;                     /* Out of scope */
    }

    pos = 7 - y / 8;                /* calculateGRAMThe insideyBytes where the coordinates are located, Each byte can be stored8Line coordinates */
    bx = y % 8;                     /* Take the remainder,convenientcalculateyThe position in the corresponding byte,And go(y)Location */
    temp = 1 << (7 - bx);           /* High position indicates high line number, getyCorrespondingbitLocation,Put thisbitSet first1 */

    if(dot)                         /* Draw solid */
    {
        OLED_GRAM[x][pos] |= temp;  
    }
    else                            /* Draw empty spots,That is notshow */
    {
        OLED_GRAM[x][pos] &= ~ temp;
    }
}

/**
 * @brief OLEDfilling Area filling
 * @note: Notice:Need to be sure: x1<=x2; y1<=y2 0<=x1<=127 0<=y1<=63
 * @param x1,y1: Starting point coordinates
 * @param x2,y2: End point coordinates
 * @param dot: 1 filling 0,Clear
 * @retval none
 */ 
void oled_fill(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;
    
    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)
        {
            oled_draw_point(x, y, dot);
        }
    }

    oled_refresh_gram();    /* Update display */
}

/**
 * @brief       In the specifiedLocationShow a character,Include some characters 
 * @param       x   : 0~127
 * @param       y   : 0~63
 * @param       size: Select a font 12/16/24
 * @param       mode: 0,Reverse white display;1,normalshow
 * @retval      none
 */ 
void oled_show_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t temp, t, t1;
    uint8_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* getFont A character corresponding to the number of bytes occupied by the dot matrix set */
    chr = chr - ' ';                                                /* getAfter offsetvalue */
    
    for (t = 0; t < csize; t ++)
    {   
        if (size == 12)
        {
            temp = atk_asc2_1206[chr][t];                           /* Call 1206 font */
        }
        else if (size == 16)
        {
            temp = atk_asc2_1608[chr][t];                           /* Call 1608 font */
        }
        else if (size == 24)
        {
            temp = atk_asc2_2412[chr][t];                           /* Call 2412 font */
        }
        else
        {
            return;                                                 /* No font library */
        }

        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
            {
                oled_draw_point(x, y, mode);
            }
            else
            {
                oled_draw_point(x, y, !mode);
            }

            temp <<= 1;
            y++;

            if ((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/**
 * @brief       Square function, m^n
 * @param       m: Base
 * @param       n: index 
 * @retval      none
 */
uint32_t oled_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       showlenNumbers 
 * @param       x,y : Start coordinates
 * @param       num : numbervalue(0 ~ 2^32)
 * @param       len : Display the number of digits
 * @param       size: Select a font 12/16/24
 * @retval      none
 */
void oled_show_num(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    
    for (t = 0; t < len; t++)                                       /* Cycling by total number of displayed digits */ 
    {
        temp = (num / oled_pow(10, len - t - 1)) % 10;              /* Get the number of the corresponding bit */

        if (enshow == 0 && t < (len - 1))                           /* There is no enabled display, and there are bits to display */
        {
            if (temp == 0)
            {
                oled_show_char(x + (size / 2)*t, y, ' ', size, 1);  /* Show spaces, stations */
                continue;                                           /* Continue to the next one */
            }
            else
            {
                enshow = 1;                                         /* Enable display */
            }
        }

        oled_show_char(x + (size / 2)*t, y, temp + '0', size, 1);   /* Show characters */
    }
} 

/**
 * @brief       Show charactersstring
 * @param       x,y : Start coordinates
 * @param       size: Select a font 12/16/24
 * @param       *p  : String pointer,Point to the first address of the string 
 * @retval      none
 */ 
void oled_show_string(uint8_t x, uint8_t y, const char *p, uint8_t size)
{
    while ((*p <= '~') && (*p >= ' '))      /* Determine whether it is an illegal character!*/
    {       
        if (x > (128 - (size / 2)))         /* Width crosses boundary */
        {
            x = 0;
            y += size;                      /* Line break */
        }
        if (y > (64 - size))                /* Highly cross-border */
        {
            y = x = 0;
            oled_clear();
        }

        oled_show_char(x, y, *p, size, 1);  /* Show a character */
        x += size / 2;                      /* ASCIIThe character width is half the width of the Chinese character */
        p++;
    }
}