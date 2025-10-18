/**
 ******************************************************************************
 * @file     15_infrared_reception.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Infrared reception experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studyInfrared reception

 * Hardware resources and pin assignments：
 * 1,     LED --> ESP32S3 IO
 *        LED --> IO1
 * 2,   UART0 --> ESP32S3 IO
 *       TXD0 --> IO43
 *       RXD0 --> IO44
 * 3,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 *        INT --> IO0(Jumper cap connection) 
 * 4, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12
 * 5,    R_IN --> ESP32S3 IO
 *         IN --> IO2

 * experimentPhenomenon：
 * 1, Press the infrared remote control，LCDKey values ​​and identifiers will be displayed

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "remote.h"

uint8_t rmt_key;
char *str = "0";

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、functions etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();             /* LEDinitialization */
    uart_init(0, 115200);   /* Serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */
    remote_init();          /* Infrared receptioninitialization */
    
    lcd_show_string(30,  50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30,  70, 200, 16, LCD_FONT_16, "REMOTE TEST", RED);
    lcd_show_string(30,  90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "KEYVAL:", RED);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "SYMBOL:", RED);
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    rmt_key  = remote_scan();

    if (rmt_key)
    {
        lcd_show_num(86, 110, rmt_key, 3, LCD_FONT_16, BLUE);   /* showkey value */

        switch (rmt_key)   /* Determine key value */
        {
            case 0:
                str = "ERROR";
                break;

            case 162:
                str = "POWER";
                break;

            case 98:
                str = "UP";
                break;

            case 2:
                str = "PLAY";
                break;

            case 226:
                str = "ALIENTEK";
                break;

            case 194:
                str = "RIGHT";
                break;

            case 34:
                str = "LEFT";
                break;

            case 224:
                str = "VOL-";
                break;

            case 168:
                str = "DOWN";
                break;

            case 144:
                str = "VOL+";
                break;

            case 104:
                str = "1";
                break;

            case 152:
                str = "2";
                break;

            case 176:
                str = "3";
                break;

            case 48:
                str = "4";
                break;

            case 24:
                str = "5";
                break;

            case 122:
                str = "6";
                break;

            case 16:
                str = "7";
                break;

            case 56:
                str = "8";
                break;

            case 90:
                str = "9";
                break;

            case 66:
                str = "0";
                break;

            case 82:
                str = "DELETE";
                break;
        }

        lcd_fill(86, 130, 116 + 8 * 8, 170 + 16, WHITE);              /* clear beforeshow */
        lcd_show_string(86, 130, 200, 16, LCD_FONT_16, str, BLUE);    /* Show SYMBOL */
    }

    LED_TOGGLE();
    delay(500);
}