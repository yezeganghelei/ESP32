/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file main.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief Touch Screen Experiment
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "led.h"
#include "ltdc.h"
#include "touch.h"

i2c_obj_t i2c0_master;

/**
 * @brief Clear the screen and display "RST" in the upper right corner
 * @param None
 * @retval None
 */
void load_draw_dialog(void)
{
    ltdc_clear(WHITE);                                                  /* Clear the screen */
    ltdc_show_string(ltdcdev.width - 24, 0, 200, 16, 16, "RST", BLUE);  /* showClear the screenarea */
}

/**
 * @brief       Draw thick lines
 * @param       x1,y1:Starting point coordinates
 * @param       x2,y2:End point coordinates
 * @param       size :Line thickness
 * @param       color:The color of the line
 * @retval      none
 */
void lcd_draw_bline(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t size, uint16_t color)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    
    if (x1 < size || x2 < size || y1 < size || y2 < size)
    {
        return;
    }
    
    delta_x = x2 - x1;                              /* Calculate coordinate increments */
    delta_y = y2 - y1;
    row = x1;
    col = y1;
    
    if (delta_x > 0)
    {
        incx = 1;                                   /* Set single step direction */
    }
    else if (delta_x == 0)
    {
        incx = 0;                                   /* Vertical line */
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
        incy = 0;                                   /* Horizontal line */
    }
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    
    if ( delta_x > delta_y)
    {
        distance = delta_x;                         /* Select the basic incremental axis */
    }
    else 
    {
        distance = delta_y;
    }
    
    for (t = 0; t <= distance + 1; t++ )            /* Line drawing output */
    {
        ltdc_draw_circle(row, col, size, color);    /* Draw points */
        xerr += delta_x ;
        yerr += delta_y ;
        
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

/* 6The color of touch points(capacitancetouchscreenuse) */
static const uint16_t POINT_COLOR_TBL[6] = {
    RED,
    GREEN,
    BLUE,
    YELLOW,
    MAGENTA,
    CYAN,
};

/**
 * @brief       Capacitive touch screen test function
 * @param       none
 * @retval      none
 */
void ctp_test(void)
{
    uint8_t t = 0;
    uint8_t i = 0;
    uint16_t lastpos[10][2];
    
    while (1)
    {
        tp_dev.scan(0);
        
        for (t = 0; t < 5; t++)
        {
            if ((tp_dev.sta) & (1 << t))
            {
                if (tp_dev.x[t] < ltdcdev.width && tp_dev.y[t] < 480)                                               /* Coordinates within the screen range */
                {
                    if (lastpos[t][0] == 0XFFFF)
                    {
                        lastpos[t][0] = tp_dev.x[t];
                        lastpos[t][1] = tp_dev.y[t];
                    }
                    
                    lcd_draw_bline(lastpos[t][0], lastpos[t][1], tp_dev.x[t], tp_dev.y[t], 2, POINT_COLOR_TBL[t]);  /* Draw lines */
                    lastpos[t][0] = tp_dev.x[t];
                    lastpos[t][1] = tp_dev.y[t];
                    
                    if (tp_dev.x[t] > (ltdcdev.width - 24) && tp_dev.y[t] < 20)
                    {
                        load_draw_dialog();                                                                         /* Clear */
                    }
                }
            }
            else 
            {
                lastpos[t][0] = 0xFFFF;
            }
        }
        
        vTaskDelay(5);
        i++;
        
        if (i % 20 == 0)
        {
            LED_TOGGLE();
        }
    }
}

/**
 * @brief       Program entry
 * @param       none
 * @retval      none
 */
void app_main(void)
{
    esp_err_t ret;
    
    ret = nvs_flash_init();             /* Initialize NVS */

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    i2c0_master = iic_init(I2C_NUM_0);  /* initializationIIC0 */
    xl9555_init(i2c0_master);           /* Initialize XL9555 */
    ltdc_init();                        /* Initialize ltdc */
    tp_dev.init();                      /* initializationtouchscreen */
    load_draw_dialog();
    ctp_test();
}