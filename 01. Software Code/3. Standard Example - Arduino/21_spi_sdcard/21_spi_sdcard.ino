/**
 ******************************************************************************
 * @file     21_spi_sdcard.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    SD experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studySDThe use of

 * Hardware resources and pin allocation：
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
 * 5,      SD --> ESP32S3 IO
 *       SDCS --> IO2
 *        SCK --> IO12
 *       MOSI --> IO11
 *       MISO --> IO13

 * experimentPhenomenon：
 * 1, LCDwill be displayedSDCard related information(capacity),You can do it by pressing the buttonSDCardtest,passSerial portAssistant to view

 * Things to note：

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"
#include "key.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "spi_sdcard.h"
#include <SD.h>

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    led_init();             /* LEDinitialization */
    key_init();             /* KEYinitialization */
    uart_init(0, 115200);   /* Serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "SD TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);

    while (sdcard_init())    /* Not detectedSDCard */    
    {
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "SD Card Error!", RED);
        delay(500);
        lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Please Check! ", RED);
        delay(500);
        LED_TOGGLE();       /* red light flashing */
    }

    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "SD Card OK    ", BLUE);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "SD Card Size:     MB", BLUE);
    lcd_show_num(30 + 13 * 8, 130, SD.cardSize() / (1024 * 1024), 5, LCD_FONT_16, BLUE); /* showSDcard capacity, converted toMBunit */
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    if (KEY == 0)
    {
        sd_test();
    }

    LED_TOGGLE();    
    delay(500);
}