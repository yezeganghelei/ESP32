/**
 ******************************************************************************
 * @file     14_iic_ap3216c.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Light environment sensor experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * Experiment purpose：Learning light environment sensorAP3216CThe use of,accomplishLight intensity(ALS)/close distance(PS)/Infrared light intensity(IR)Measurements such as

 * Hardware resources and pin assignments：
 * 1,   UART0 --> ESP32S3 IO
 *       TXD0 --> IO43
 *       RXD0 --> IO44
 * 2,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> IO0(Jumper cap connection) 
 * 3, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12
 * 4, AP3216C --> ESP32S3 IO
 *        SCL --> 40
 *        SDA --> 41
 *        INT --> XL9555_P00

 * Experimental phenomenon：
 * 1, LCDThe light environment sensor will be displayedALS+PS+IRdata

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "ap3216c.h"

uint16_t ir, als, ps;       /* Light intensity(ALS)/close distance(PS)/Infrared light intensity(IR) */

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、functions etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    uart_init(0, 115200);   /* Serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "AP3216C TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    while (ap3216c_init())  /* AP3216C not detected */
    {
        lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "AP3216C Check Failed!", RED);
        delay(500);
    }
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "AP3216C Ready!", RED);
        
    lcd_show_string(30, 160, 200, 16, LCD_FONT_16, " IR:", RED);
    lcd_show_string(30, 180, 200, 16, LCD_FONT_16, " PS:", RED);
    lcd_show_string(30, 200, 200, 16, LCD_FONT_16, "ALS:", RED);
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    ap3216c_read_data(&ir, &ps, &als);                      /* readdata  */

    lcd_show_num(30 + 32, 160, ir, 5, LCD_FONT_16, BLUE);   /* Display IR data */
    lcd_show_num(30 + 32, 180, ps, 5, LCD_FONT_16, BLUE);   /* showPSdata */
    lcd_show_num(30 + 32, 200, als, 5, LCD_FONT_16, BLUE);  /* Display ALS data  */

    delay(500); 
}