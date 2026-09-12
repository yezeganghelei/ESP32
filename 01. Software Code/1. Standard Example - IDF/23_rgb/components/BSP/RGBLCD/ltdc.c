/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file ltdc.c
 * @author
 * @version V1.0
 * @date 2023-12-1
 * @brief LTDC driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "ltdc.h"
#include "ltdcfont.h"

static const char *TAG = "ltdc";
esp_lcd_panel_handle_t panel_handle = NULL;                         /* RGB LCD handle */
static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;     /* portMUX_TYPE spinlock variable for critical-section protection */
uint32_t g_back_color  = 0xFFFF;                                    /* Background color */

/* LTDC management parameters */
_ltdc_dev ltdcdev;

/**
 * @brief       Read the LTDC panel ID
 * @note        Use the highest bits of the LCD RGB lines (R7, G7, B7) to identify the panel ID
 *              PG6 = R7(M0); PI2 = G7(M1); PI7 = B7(M2);
 *              M2:M1:M0
 *              0:0:0  4.3-inch 480*272 RGB screen, ID = 0X4342
 *              1:0:0  4.3-inch 800*480 RGB screen, ID = 0X4384
 * @param       none
 * @retval      0, illegal; 
 *              other, LCD ID
 */
uint16_t ltdc_panelid_read(void)
{
    uint8_t idx = 0;
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* Input mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* Enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* Disable pull-down */
    gpio_init_struct.pin_bit_mask = 1ull << GPIO_LCD_B7;
    gpio_config(&gpio_init_struct);                         /* Configure GPIO */

    gpio_init_struct.pin_bit_mask = 1ull << GPIO_LCD_R7 || 1ull << GPIO_LCD_G7;
    gpio_config(&gpio_init_struct);                         /* Configure GPIO */

    idx  = (uint8_t)gpio_get_level(GPIO_LCD_R7);            /* Read M0 */
    idx |= (uint8_t)gpio_get_level(GPIO_LCD_G7) << 1;       /* Read M1 */
    idx |= (uint8_t)gpio_get_level(GPIO_LCD_B7) << 2;       /* Read M2 */

    switch (idx)
    {
        case 0 : 
            return 0X4342;                                  /* 4.3-inch screen, 480*272 resolution */
        case 4 : 
            return 0X4384;                                  /* 4.3-inch screen, 800*480 resolution */
        default : 
            return 0;
    }
}

/**
 * @brief       Initialize LTDC
 * @param       none
 * @retval      none
 */
void ltdc_init(void)
{
    panel_handle = NULL;
    ltdcdev.id = ltdc_panelid_read();           /* Read LCD panel ID */

    if (ltdcdev.id == 0X4342)                   /* 4.3-inch screen, 480*272 RGB */
    {
        ltdcdev.pwidth = 480;                   /* Panel width (unit: pixels) */
        ltdcdev.pheight = 272;                  /* Panel height (unit: pixels) */
        ltdcdev.hsw = 1;                        /* Horizontal synchronization width */
        ltdcdev.vsw = 1;                        /* Vertical synchronization width */
        ltdcdev.hbp = 40;                       /* Horizontal back porch */
        ltdcdev.vbp = 8;                        /* Vertical back porch */
        ltdcdev.hfp = 5;                        /* Horizontal front porch */
        ltdcdev.vfp = 8;                        /* Vertical front porch */
        ltdcdev.pclk_hz = 9 * 1000 * 1000;      /* Set the pixel clock to 9 MHz */
    }
    else if (ltdcdev.id == 0X4384)
    {
        ltdcdev.pwidth = 800;                   /* Panel width (unit: pixels) */
        ltdcdev.pheight = 480;                  /* Panel height (unit: pixels) */
        ltdcdev.hbp = 88;                       /* Horizontal back porch */
        ltdcdev.hfp = 40;                       /* Horizontal front porch */
        ltdcdev.hsw = 48;                       /* Horizontal synchronization width */
        ltdcdev.vbp = 32;                       /* Vertical back porch */
        ltdcdev.vfp = 13;                       /* Vertical front porch */
        ltdcdev.vsw = 3;                        /* Vertical synchronization width */
        ltdcdev.pclk_hz = 18 * 1000 * 1000;     /* Set the pixel clock to 18 MHz */
    }

    /* Configure RGB parameters */
    esp_lcd_rgb_panel_config_t panel_config = { /* RGB LCD configuration structure */
        .data_width = 16,                       /* Data width is 16 bits */
        .psram_trans_align = 64,                /* Alignment of buffers allocated in PSRAM */
        .clk_src = LCD_CLK_SRC_PLL160M,         /* RGB LCD peripheral clock source */
        .disp_gpio_num = GPIO_NUM_NC,           /* Display control signal; set to -1 if unused */
        .pclk_gpio_num = GPIO_LCD_PCLK,         /* PCLK signal pin */
        .hsync_gpio_num = GPIO_NUM_NC,          /* HSYNC signal pin; unused in DE mode */
        .vsync_gpio_num = GPIO_NUM_NC,          /* VSYNC signal pin; unused in DE mode */
        .de_gpio_num = GPIO_LCD_DE,             /* DE signal pin */
        .data_gpio_nums = {                     /* Data line pins */
            GPIO_LCD_B3, GPIO_LCD_B4, GPIO_LCD_B5, GPIO_LCD_B6, GPIO_LCD_B7,
            GPIO_LCD_G2, GPIO_LCD_G3, GPIO_LCD_G4, GPIO_LCD_G5, GPIO_LCD_G6, GPIO_LCD_G7,
            GPIO_LCD_R3, GPIO_LCD_R4, GPIO_LCD_R5, GPIO_LCD_R6, GPIO_LCD_R7,
        },
        .timings = {                            /* RGB LCD timing parameters */
            .pclk_hz = ltdcdev.pclk_hz,         /* Pixel clock frequency */
            .h_res = ltdcdev.pwidth,            /* Horizontal resolution, i.e. number of pixels per line */
            .v_res = ltdcdev.pheight,           /* Vertical resolution, i.e. number of rows per frame */
            .hsync_back_porch = ltdcdev.hbp,    /* Number of PCLKs between hsync and the start of active row data (horizontal back porch) */
            .hsync_front_porch = ltdcdev.hfp,   /* Horizontal front porch; number of PCLKs between the end of active data and the next hsync */
            .hsync_pulse_width = ltdcdev.vsw,   /* Vertical sync pulse width (unit: rows) */
            .vsync_back_porch = ltdcdev.vbp,    /* Vertical back porch; number of idle rows between vsync and the start of the frame */
            .vsync_front_porch = ltdcdev.vfp,   /* Vertical front porch; number of idle rows between the end of the frame and the next vsync */
            .vsync_pulse_width = ltdcdev.hsw,   /* Horizontal sync pulse width (unit: PCLK periods) */
            .flags.pclk_active_neg = true,      /* RGB data is timed on the falling edge */
        },
        .flags.fb_in_psram = true,              /* Allocate framebuffers in PSRAM */
        .bounce_buffer_size_px = (ltdcdev.id == 0X4384) ? 480 * 10 : 272 * 10,  /* Avoid jitter when writing to SPI flash */
    };

    /* Create the RGB object */
    esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
    /* Reset the RGB screen */
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    /* Initialize RGB */
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    /* Set landscape orientation */
    ltdc_display_dir(1);
    /* Clear the screen to a color */
    ltdc_clear(WHITE);
    /* Turn on the backlight */
    LCD_BL(1);
}

/**
 * @brief       Clear the screen
 * @param       color:Clear color
 * @retval      none
 */
void ltdc_clear(uint16_t color)
{
    uint16_t *buffer = heap_caps_malloc(ltdcdev.width * sizeof(uint16_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);

    if (NULL == buffer)
    {
        ESP_LOGE(TAG, "Memory for bitmap is not enough");
    }
    else
    {
        for (uint16_t i = 0; i < ltdcdev.width; i++)
        {
            buffer[i] = color;
        }
        
        for (uint16_t y = 0; y < ltdcdev.height; y++)
        {   /* Use taskENTER_CRITICAL()/taskEXIT_CRITICAL() to protect the pixel-drawing process; task scheduling is disabled */
            taskENTER_CRITICAL(&my_spinlock);   /* Disable interrupts */
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, ltdcdev.width, y + 1, buffer);
            taskEXIT_CRITICAL(&my_spinlock);    /* Re-enable interruptss */
        }
        
        heap_caps_free(buffer);
    }
}

/**
 * @brief       Convert RGB888 to RGB565
 * @param       r:red
 * @param       g:green
 * @param       b:blue
 * @retval      Return the RGB565 color value
 */
uint16_t ltdc_rgb888_to_565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/**
 * @brief       Set the LTDC display orientation
 * @param       dir: 0, portrait; 1, landscape
 * @retval      none
 */
void ltdc_display_dir(uint8_t dir)
{
    ltdcdev.dir = dir;              /* Display orientation */

    if (ltdcdev.dir == 0)           /* Portrait */
    {
        ltdcdev.width = ltdcdev.pheight;
        ltdcdev.height = ltdcdev.pwidth;
        esp_lcd_panel_swap_xy(panel_handle, true);          /* Swap the X and Y axes */ 
        esp_lcd_panel_mirror(panel_handle, false, true);    /* Mirror the screen on the Y axis */
    }
    else if (ltdcdev.dir == 1)      /* Landscape */
    {
        ltdcdev.width = ltdcdev.pwidth;
        ltdcdev.height = ltdcdev.pheight;
        esp_lcd_panel_swap_xy(panel_handle, false);         /* Do not swap the X and Y axes */
        esp_lcd_panel_mirror(panel_handle, false, false);   /* Do not mirror either axis */
    }
}

/**
 * @brief       LTDC pixel-drawing function
 * @param       x,y     : Pixel coordinates
 * @param       color   :Color value
 * @retval      none
 */
void ltdc_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    taskENTER_CRITICAL(&my_spinlock);
    esp_lcd_panel_draw_bitmap(panel_handle, x, y, x + 1, y + 1, &color);
    taskEXIT_CRITICAL(&my_spinlock);
}

/**
 * @brief       Fill the specified area with a color
 * @note        This function only supports filling a color array in uint16_t RGB565 format.
 *              (sx,sy),(ex,ey): Diagonal coordinates of the rectangle; area = (ex - sx + 1) * (ey - sy + 1)
 *              Note: sx and ex must not exceed ltdcdev.width - 1; sy and ey must not exceed ltdcdev.height - 1
 * @param       sx,sy: Start coordinates
 * @param       ex,ey: End coordinates
 * @param       color:Start address of the color array
 * @retval      none
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    /* Make sure the coordinates are within the LCD range */
    if (sx < 0 || sy < 0 || ex > ltdcdev.width || ey > ltdcdev.height)
    {
        return; /* Coordinate is outside the LCD range; do not fill */
    }  
      
    /* Ensure the start coordinates are less than the end coordinates */
    if (sx > ex || sy > ey)
    {
        return; /* Invalid fill area; do not fill */
    }  
      
    /* Ensure the filled area is within the LCD range */
    sx = fmax(0, sx);
    sy = fmax(0, sy);
    ex = fmin(ltdcdev.width - 1, ex);
    ey = fmin(ltdcdev.height - 1, ey);
      
    /* Start filling the color */
    for (int i = sx; i <= ex; i++)
    {
        for (int j = sy; j <= ey; j++)
        {
            /* Set the RGB value for the corresponding pixel on the LCD */
            ltdc_draw_point(i, j, color);
        }
    }
}

/**
 * @brief       Display a monochrome icon
 * @param       x,y,width,height: Coordinates and dimensions
 * @param       icosbase: Icon bitmap data
 * @param       color:Pixel color
 * @param       bkcolor:Background color
 * @retval      none
 */
void ltdc_app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor)
{
    uint16_t rsize;
    uint16_t i,j;
    uint8_t temp;
    uint8_t t = 0;
    uint16_t x0 = x;//Save the X coordinate
    rsize = width / 8 + ((width % 8) ? 1 : 0);//Number of bytes per line

    for (i = 0;i < rsize * height;i++)
    {
        temp = icosbase[i];

        for(j = 0;j < 8;j++)
        {
            if (temp & 0x80)
            {
                ltdc_draw_point(x,y,color);
            }
            else
            {
                ltdc_draw_point(x,y,bkcolor);
            }

            temp <<= 1;
            x++;
            t++;        //Width Counter

            if (t == width)
            {
                t=0;
                x=x0;
                y++;
                break;
            }
        }
    }
}

/**
 * @brief Draw a line
 * @param x1,y1: Start coordinates
 * @param x2,y2: End coordinates
 * @param color:The color of the line
 * @retval none
 */
void ltdc_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    delta_x = x2 - x1;      /* Calculate coordinate increments */
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if (delta_x > 0)
    {
        incx = 1;           /* Set single step direction */
    }
    else if (delta_x == 0)
    {
        incx = 0;           /* Vertical line */
    }
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
    {
        incy = 1;
    }
    else if (delta_y == 0)
    {
        incy = 0;            /* Horizontal line */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if ( delta_x > delta_y)
    {
        distance = delta_x; /* Select the dominant increment axis */
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++)     /* Draw the line */
    {
        ltdc_draw_point(row, col, color);   /* Draw pixel */
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief       Draw a rectangle
 * @param       x1,y1: Start coordinates
 * @param       x2,y2: End coordinates
 * @param       color: Fill color
 * @retval      none
 */
void ltdc_draw_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color)
{
    ltdc_draw_line(x0, y0, x1, y0,color);
    ltdc_draw_line(x0, y0, x0, y1,color);
    ltdc_draw_line(x0, y1, x1, y1,color);
    ltdc_draw_line(x1, y0, x1, y1,color);
}

/**
 * @brief       Draw a circle
 * @param       x0,y0: Circle center coordinates
 * @param       r: Radius
 * @param       color:Circle color
 * @retval      none
 */
void ltdc_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* Decision variable for one pixel */

    while (a <= b)
    {
        ltdc_draw_point(x0 + a, y0 - b, color);  /* 5 */
        ltdc_draw_point(x0 + b, y0 - a, color);  /* 0 */
        ltdc_draw_point(x0 + b, y0 + a, color);  /* 4 */
        ltdc_draw_point(x0 + a, y0 + b, color);  /* 6 */
        ltdc_draw_point(x0 - a, y0 + b, color);  /* 1 */
        ltdc_draw_point(x0 - b, y0 + a, color);
        ltdc_draw_point(x0 - a, y0 - b, color);  /* 2 */
        ltdc_draw_point(x0 - b, y0 - a, color);  /* 7 */
        a++;

        /* Draw the circle using Bresenham's algorithm */
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}

/**
 * @brief       Display a character at the specified position
 * @param       x,y: Coordinates
 * @param       chr  :Characters to be displayed:" "--->"~"
 * @param       size :Font size 12/16/24/32
 * @param       mode :Overlay mode (1); non-overlay mode (0);
 * @param       color: Font color
 * @retval      none
 */
void ltdc_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* Number of bytes occupied by one character in the selected font */
    chr = (char)chr - ' ';      /* Get the offset value (the ASCII font library starts at space, so subtracting ' ' selects the corresponding character) */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[(uint8_t)chr];     /* Call 1206 font */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[(uint8_t)chr];     /* Call 1608 font */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[(uint8_t)chr];     /* Use 2412 font */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[(uint8_t)chr];     /* Use 3216 font */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];                                    /* Get the dot-matrix data of the character */

        for (t1 = 0; t1 < 8; t1++)                          /* 8 pixels per byte */
        {
            if (temp & 0x80)                                /* Valid pixel; display it */
            {
                ltdc_draw_point(x, y, color);               /* Draw the pixel to display it */
            }
            else if (mode == 0)                             /* Invalid pixel, do not display */
            {
                ltdc_draw_point(x, y, g_back_color);        /* Draw the background color; this pixel is not displayed (background color is a global variable) */
            }

            temp <<= 1;                                     /* Shift to get the state of the next bit */
            y++;

            if (y >= ltdcdev.height)return;                 /* Out of range */

            if ((y - y0) == size)                           /* Has a full column been displayed? */
            {
                y = y0;                                     /* Reset the Y coordinate */
                x++;                                        /* Increment the X coordinate */
                
                if (x >= ltdcdev.width)
                {
                    return;                                 /* X coordinate out of range */
                }

                break;
            }
        }
    }
}

/**
 * @brief       Power function, m^n
 * @param       m: Base
 * @param       n: Exponent
 * @retval      m to the power of n
 */
static uint32_t ltdc_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while (n--)
    {
        result *= m;
    }

    return result;
}

/**
 * @brief       Display len digits
 * @param       x,y     : Start coordinates
 * @param       num     : Value (0 ~ 2^32)
 * @param       len     : Number of digits to display
 * @param       size    : Font size 12/16/24/32
 * @param       color   : Font color
 * @retval      none
 */
void ltdc_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                               /* Loop over the total number of digits */
    {
        temp = (num / ltdc_pow(10, len - t - 1)) % 10;                      /* Get the digit at the corresponding position */

        if (enshow == 0 && t < (len - 1))                                   /* Display is not yet enabled and there are more digits to show. */
        {
            if (temp == 0)
            {
                ltdc_show_char(x + (size / 2) * t, y, ' ', size, 0, color); /* Display a space as placeholder */
                continue;                                                   /* Continue to the next digit */
            }
            else
            {
                enshow = 1;                                                 /* Enable display */
            }
        }

        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);  /* Show characters */
    }
}

/**
 * @brief       Extended digit display (also shows leading zeros)
 * @param       x,y     : Start coordinates
 * @param       num     : Value (0 ~ 2^32)
 * @param       len     : Number of digits to display
 * @param       size    : Font size 12/16/24/32
 * @param       mode    : Display mode
 *              [7]: 0 = no fill, 1 = fill with 0.
 *              [6:1]: Reserved
 *              [0]: 0 = non-overlay, 1 = overlay.
 * @param       color   : Font color
 * @retval      none
 */
void ltdc_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                                               /* Loop over the total number of digits */
    {
        temp = (num / ltdc_pow(10, len - t - 1)) % 10;                                      /* Get the digit at the corresponding position */

        if (enshow == 0 && t < (len - 1))                                                   /* Display is not yet enabled and there are more digits to show. */
        {
            if (temp == 0)
            {
                if (mode & 0x80)                                                            /* Leading positions should be filled with 0 */
                {
                    ltdc_show_char(x + (size / 2) * t, y, '0', size, mode & 0x01, color);   /* Fill with 0 */
                }
                else
                {
                    ltdc_show_char(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);   /* Fill with space */
                }

                continue;
            }
            else
            {
                enshow = 1;                                                                 /* Enable display */
            }

        }

        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

/**
 * @brief       Display a string of characters
 * @param       x,y         :Start coordinates
 * @param       width,height:Area size
 * @param       size        :Select a font 12/16/24/32
 * @param       p           :Start address of the string
 * @param       color       :Font color
 * @retval      none
 */
void ltdc_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
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

        if (y >= height)
        {
            break;                       /* Exit */
        }

        ltdc_show_char(x, y, *p, size, 1, color);
        x += size / 2;
        p++;
    }
}