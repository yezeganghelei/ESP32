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
esp_lcd_panel_handle_t panel_handle = NULL;                         /* RGBLCDHandle */
static portMUX_TYPE my_spinlock = portMUX_INITIALIZER_UNLOCKED;     /* definitionportMUX_TYPEtypeofSpinlock variable,For critical area protection */
uint32_t g_back_color  = 0xFFFF;                                    /* Background color */

/* manageLTDC重wantparameter */
_ltdc_dev ltdcdev;

/**
 * @brief       LTDCRead panelID
 * @note        profituseLCD RGBWireof最highBit(R7,G7,B7)To identifypanelID
 *              PG6 = R7(M0); PI2 = G7(M1); PI7 = B7(M2);
 *              M2:M1:M0
 *              0 :0 :0     4.3 inch480*272  RGBscreen,ID = 0X4342
 *              1 :0 :0     4.3 inch800*480  RGBscreen,ID = 0X4384
 * @param       none
 * @retval      0, illegal; 
 *              other, LCD ID
 */
uint16_t ltdc_panelid_read(void)
{
    uint8_t idx = 0;
    gpio_config_t gpio_init_struct = {0};

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;         /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT;                /* Input and output mode */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;       /* Enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;  /* Disabled pull-down */
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
            return 0X4342;                                  /* 4.3inchscreen, 480*272Resolution */
        case 4 : 
            return 0X4384;                                  /* 4.3inchscreen, 800*480Resolution */
        default : 
            return 0;
    }
}

/**
 * @brief       initializationltdc
 * @param       none
 * @retval      none
 */
void ltdc_init(void)
{
    panel_handle = NULL;
    ltdcdev.id = ltdc_panelid_read();           /* Read LCD panel ID */

    if (ltdcdev.id == 0X4342)                   /* 4.3inchscreen, 480*272 RGBscreen */
    {
        ltdcdev.pwidth = 480;                   /* panelwidth,oneBit:Pixels */
        ltdcdev.pheight = 272;                  /* Panel height, single Bit:Pixels */
        ltdcdev.hsw = 1;                        /* Horizontal synchronization width */
        ltdcdev.vsw = 1;                        /* Vertical synchronization width */
        ltdcdev.hbp = 40;                       /* Horizontal back porch */
        ltdcdev.vbp = 8;                        /* Vertical rear porch */
        ltdcdev.hfp = 5;                        /* Horizontal front porch */
        ltdcdev.vfp = 8;                        /* Vertical front porch */
        ltdcdev.pclk_hz = 9 * 1000 * 1000;      /* set upPixelshourbell 9Mhz */
    }
    else if (ltdcdev.id == 0X4384)
    {
        ltdcdev.pwidth = 800;                   /* panelwidth,oneBit:Pixels */
        ltdcdev.pheight = 480;                  /* Panel height, single Bit:Pixels */
        ltdcdev.hbp = 88;                       /* Horizontal back porch */
        ltdcdev.hfp = 40;                       /* Horizontal front porch */
        ltdcdev.hsw = 48;                       /* Horizontal synchronization width */
        ltdcdev.vbp = 32;                       /* Vertical rear porch */
        ltdcdev.vfp = 13;                       /* Vertical front porch */
        ltdcdev.vsw = 3;                        /* Vertical synchronization width */
        ltdcdev.pclk_hz = 18 * 1000 * 1000;     /* set upPixelshourbell 18Mhz */
    }

    /* ConfigurationRGBparameter */
    esp_lcd_rgb_panel_config_t panel_config = { /* RGBLCDConfigure structure */
        .data_width = 16,                       /* numberaccording towidthfor16Bit */
        .psram_trans_align = 64,                /* Alignment of buffers allocated in PSRAM */
        .clk_src = LCD_CLK_SRC_PLL160M,         /* RGBLCD peripheral clock source */
        .disp_gpio_num = GPIO_NUM_NC,           /* useAtshowControl signal,Set as not used-1 */
        .pclk_gpio_num = GPIO_LCD_PCLK,         /* PCLKSignal pin */
        .hsync_gpio_num = GPIO_NUM_NC,          /* HSYNCSignal pin,DEMode is not used */
        .vsync_gpio_num = GPIO_NUM_NC,          /* VSYNCSignal pin,DEMode is not used */
        .de_gpio_num = GPIO_LCD_DE,             /* DESignal pin */
        .data_gpio_nums = {                     /* numberaccording toWire引脚 */
            GPIO_LCD_B3, GPIO_LCD_B4, GPIO_LCD_B5, GPIO_LCD_B6, GPIO_LCD_B7,
            GPIO_LCD_G2, GPIO_LCD_G3, GPIO_LCD_G4, GPIO_LCD_G5, GPIO_LCD_G6, GPIO_LCD_G7,
            GPIO_LCD_R3, GPIO_LCD_R4, GPIO_LCD_R5, GPIO_LCD_R6, GPIO_LCD_R7,
        },
        .timings = {                            /* RGBLCDhour sequence parameter */
            .pclk_hz = ltdcdev.pclk_hz,         /* Pixelshourbell频率 */
            .h_res = ltdcdev.pwidth,            /* Horizontal resolution，i.e. in one lineofPixelsnumber */
            .v_res = ltdcdev.pheight,           /* Vertical resolution, that is, ofNumber of rows in frame */
            .hsync_back_porch = ltdcdev.hbp,    /* PCLK number between horizontal back porch, hsync and row activity data start */
            .hsync_front_porch = ltdcdev.hfp,   /* Horizontal front porch，ActivitynumberendsandNexthsyncBetweenPCLKnumber */
            .hsync_pulse_width = ltdcdev.vsw,   /* Vertical synchronization width, single Bit: Number of rows */
            .vsync_back_porch = ltdcdev.vbp,    /* Vertical rear porch, vsyncand frame starts Betweennone effect Number of rows */
            .vsync_front_porch = ltdcdev.vfp,   /* Vertical front porch, End of frame and nextvsyncBetweennone effect Number of rows */
            .vsync_pulse_width = ltdcdev.hsw,   /* Horizontal synchronization width, unit: PCLK period */
            .flags.pclk_active_neg = true,      /* RGB data is timed on the falling edge */
        },
        .flags.fb_in_psram = true,              /* Allocating framebuffers in PSRAM */
        .bounce_buffer_size_px = (ltdcdev.id == 0X4384) ? 480 * 10 : 272 * 10,  /* Solve writingspiflashhour,Jitter problem */
    };

    /* createRGBObject */
    esp_lcd_new_rgb_panel(&panel_config, &panel_handle);
    /* complexBitRGBscreen */
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    /* Initialize RGB */
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    /* set upHorizontal screen */
    ltdc_display_dir(1);
    /* Clear the screen as color */
    ltdc_clear(WHITE);
    /* Turn on the backlight */
    LCD_BL(1);
}

/**
 * @brief       Clear the screen
 * @param       color:Cleared color
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
        {   /* makeusetaskENTER_CRITICAL()andtaskEXIT_CRITICAL()ProtectDraw pointsprocess,Task scheduling is prohibited */
            taskENTER_CRITICAL(&my_spinlock);   /* Blocking interrupt */
            esp_lcd_panel_draw_bitmap(panel_handle, 0, y, ltdcdev.width, y + 1, buffer);
            taskEXIT_CRITICAL(&my_spinlock);    /* Re-enable interrupt */
        }
        
        heap_caps_free(buffer);
    }
}

/**
 * @brief       RGB888changeRGB565
 * @param       r:red
 * @param       g:green
 * @param       b:blue
 * @retval      returnRGB565Color value
 */
uint16_t ltdc_rgb888_to_565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/**
 * @brief       LTDCShow directionset up
 * @param       dir:0,Vertical screen；1,Horizontal screen
 * @retval      none
 */
void ltdc_display_dir(uint8_t dir)
{
    ltdcdev.dir = dir;              /* Show direction */

    if (ltdcdev.dir == 0)           /* Vertical screen */
    {
        ltdcdev.width = ltdcdev.pheight;
        ltdcdev.height = ltdcdev.pwidth;
        esp_lcd_panel_swap_xy(panel_handle, true);          /* exchangeXandYaxis */ 
        esp_lcd_panel_mirror(panel_handle, false, true);    /* To the screenYAxis mirroring */
    }
    else if (ltdcdev.dir == 1)      /* Horizontal screen */
    {
        ltdcdev.width = ltdcdev.pwidth;
        ltdcdev.height = ltdcdev.pheight;
        esp_lcd_panel_swap_xy(panel_handle, false);         /* No需wantexchangeXandYaxis */
        esp_lcd_panel_mirror(panel_handle, false, false);   /* To the screenXYThe axis is not mirrored */
    }
}

/**
 * @brief       LTDCDraw pointsletternumber
 * @param       x,y     :Write coordinates
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
 * @brief       Fill in the specified color block in the specified area
 * @note        This letternumberSupported onlyuint16_t,RGB565FormatofcolornumberGroupfilling.
 *              (sx,sy),(ex,ey):Fill the diagonal coordinates of rectangles,Area sizefor:(ex - sx + 1) * (ey - sy + 1)
 *              Notice:sx,ex,Can't be greater thanltdcdev.width - 1; sy,ey,Can't be greater thanltdcdev.height - 1
 * @param       sx,sy:Start coordinates
 * @param       ex,ey:Finishcoordinate
 * @param       color:The color array first address
 * @retval      none
 */
void ltdc_color_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    /* Make sure the coordinates are within the LCD range */
    if (sx < 0 || sy < 0 || ex > ltdcdev.width || ey > ltdcdev.height)
    {
        return; /* coordinateMore thanLCDscope，Not executedfilling */
    }  
      
    /* make sureStart coordinatesLess than endcoordinate */
    if (sx > ex || sy > ey)
    {
        return; /* Invalid fill area，Not executedfilling */
    }  
      
    /* make surefillingComplete areaexistLCDscopeInside */
    sx = fmax(0, sx);
    sy = fmax(0, sy);
    ex = fmin(ltdcdev.width - 1, ex);
    ey = fmin(ltdcdev.height - 1, ey);
      
    /* startfillingcolor */
    for (int i = sx; i <= ex; i++)
    {
        for (int j = sy; j <= ey; j++)
        {
            /* set upRGBValue toLCDofcorrespondBitSet */
            ltdc_draw_point(i, j, color);
        }
    }
}

/**
 * @brief       showMonochrome icon
 * @param       x,y,width,height:coordinateand rulerinch
 * @param       icosbase:Click on itBitSet
 * @param       color:Color of dots
 * @param       bkcolor:Background color
 * @retval      none
 */
void ltdc_app_show_mono_icos(uint16_t x,uint16_t y,uint8_t width,uint8_t height,uint8_t *icosbase,uint16_t color,uint16_t bkcolor)
{
    uint16_t rsize;
    uint16_t i,j;
    uint8_t temp;
    uint8_t t = 0;
    uint16_t x0 = x;//reservexofBit
    rsize = width / 8 + ((width % 8) ? 1 : 0);//Each lineofCharacter节number

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
 * @brief Draw lines
 * @param x1,y1: Starting point coordinate
 * @param x2,y2: End point coordinate
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
        distance = delta_x; /* Select the basic incrementcoordinateaxis */
    }
    else
    {
        distance = delta_y;
    }

    for (t = 0; t <= distance + 1; t++)     /* Draw linesoutput */
    {
        ltdc_draw_point(row, col, color);   /* Draw points */
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
 * @param       x1,y1   starting pointcoordinate
 * @param       x2,y2   endcoordinate
 * @param       color fillingcolor
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
 * @param       x0,y0:Circle center coordinates
 * @param       r    :radius
 * @param       color:Round color
 * @retval      none
 */
void ltdc_draw_circle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);       /* JudgmentOne dotBitSetofLogo */

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

        /* makeuseBresenhamAlgorithm drawing circles */
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
 * @brief       existrefer to定BitSetshow一indivualCharacter符
 * @param       x,y  :coordinate
 * @param       chr  :Characters to be displayed:" "--->"~"
 * @param       size :Font size 12/16/24/32
 * @param       mode :Overlay method(1); NoOverlay method(0);
 * @param       color:Font color
 * @retval      none
 */
void ltdc_show_char(uint16_t x, uint16_t y, char chr, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = 0;
    uint8_t *pfont = 0;

    csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); /* Get the number of bytes occupied by a character in the font corresponding to the dot matrix set */
    chr = (char)chr - ' ';      /* 得到偏shift后ofvalue（ASCIIFont library starts with spaces to get the modulus，so-' 'It is the font library for corresponding characters） */

    switch (size)
    {
        case 12:
            pfont = (uint8_t *)asc2_1206[(uint8_t)chr];     /* Call 1206 font */
            break;

        case 16:
            pfont = (uint8_t *)asc2_1608[(uint8_t)chr];     /* Call 1608 font */
            break;

        case 24:
            pfont = (uint8_t *)asc2_2412[(uint8_t)chr];     /* Call2412Font */
            break;

        case 32:
            pfont = (uint8_t *)asc2_3216[(uint8_t)chr];     /* Call3216Font */
            break;

        default:
            return ;
    }

    for (t = 0; t < csize; t++)
    {
        temp = pfont[t];                                    /* 获取Character符oflatticenumberaccording to */

        for (t1 = 0; t1 < 8; t1++)                          /* One byte8One dot */
        {
            if (temp & 0x80)                                /* Valid points, need to be displayed */
            {
                ltdc_draw_point(x, y, color);               /* Draw pointscome out,wantshowthisOne dot */
            }
            else if (mode == 0)                             /* Invalid point, not show */
            {
                ltdc_draw_point(x, y, g_back_color);        /* paintingBackground color,This point is not displayed(NoticeBackground colorControlled by global variables) */
            }

            temp <<= 1;                                     /* shiftBit, To get the next oneBitofstate */
            y++;

            if (y >= ltdcdev.height)return;                 /* Exceeded the area */

            if ((y - y0) == size)                           /* A column has been displayed? */
            {
                y = y0;                                     /* yCoordinate reset */
                x++;                                        /* xcoordinateIncrement */
                
                if (x >= ltdcdev.width)
                {
                    return;                                 /* xcoordinateExceeded the area */
                }

                break;
            }
        }
    }
}

/**
 * @brief       Square function, m^n
 * @param       m:endnumber
 * @param       n:refer tonumber
 * @retval      mofnTo the power
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
 * @brief       showlenindivualnumberCharacter
 * @param       x,y     :Start coordinates
 * @param       num     :numbervalue(0 ~ 2^32)
 * @param       len     :shownumberCharacterofBitnumber
 * @param       size    :Select a font 12/16/24/32
 * @param       color   :Font color
 * @retval      none
 */
void ltdc_show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                               /* Loop by total showBitnumber */
    {
        temp = (num / ltdc_pow(10, len - t - 1)) % 10;                      /* Get the correspondingBitofnumberCharacter */

        if (enshow == 0 && t < (len - 1))                                   /* Show is not enabled, and there are still bits to show. */
        {
            if (temp == 0)
            {
                ltdc_show_char(x + (size / 2) * t, y, ' ', size, 0, color); /* Show space, account for Bit */
                continue;                                                   /* Continue to the next Bit */
            }
            else
            {
                enshow = 1;                                                 /* make能show */
            }
        }

        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, 0, color);  /* Show characters */
    }
}

/**
 * @brief       ExtendedshowlenindivualnumberCharacter(highBityes0alsoshow)
 * @param       x,y     :Start coordinates
 * @param       num     :numbervalue(0 ~ 2^32)
 * @param       len     :shownumberCharacterofBitnumber
 * @param       size    :Select a font 12/16/24/32
 * @param       mode    :showmodel
 *              [7]:0,Nofilling;1,filling0.
 *              [6:1]:reserve
 *              [0]:0,Non-overlappingshow;1,Overlayshow.
 * @param       color   :Font color
 * @retval      none
 */
void ltdc_show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode, uint16_t color)
{
    uint8_t t, temp;
    uint8_t enshow = 0;

    for (t = 0; t < len; t++)                                                               /* Loop by total showBitnumber */
    {
        temp = (num / ltdc_pow(10, len - t - 1)) % 10;                                      /* Get the correspondingBitofnumberCharacter */

        if (enshow == 0 && t < (len - 1))                                                   /* Show is not enabled, and there are still bits to show. */
        {
            if (temp == 0)
            {
                if (mode & 0x80)                                                            /* highBit需wantfilling0 */
                {
                    ltdc_show_char(x + (size / 2) * t, y, '0', size, mode & 0x01, color);   /* use0 accounts for Bit */
                }
                else
                {
                    ltdc_show_char(x + (size / 2) * t, y, ' ', size, mode & 0x01, color);   /* useSpacesoccupyBit */
                }

                continue;
            }
            else
            {
                enshow = 1;                                                                 /* make能show */
            }

        }

        ltdc_show_char(x + (size / 2) * t, y, temp + '0', size, mode & 0x01, color);
    }
}

/**
 * @brief       Show charactersstring
 * @param       x,y         :Start coordinates
 * @param       width,height:Area size
 * @param       size        :Select a font 12/16/24/32
 * @param       p           :String head address
 * @param       color       :Font color
 * @retval      none
 */
void ltdc_show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, char *p, uint16_t color)
{
    uint8_t x0 = x;
    
    width += x;
    height += y;

    while ((*p <= '~') && (*p >= ' '))   /* Determine whether it is an illegal character! */
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }

        if (y >= height)
        {
            break;                       /* quit */
        }

        ltdc_show_char(x, y, *p, size, 1, color);
        x += size / 2;
        p++;
    }
}