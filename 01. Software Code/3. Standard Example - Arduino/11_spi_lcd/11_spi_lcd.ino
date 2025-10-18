/**
 ******************************************************************************
 * @file     11_spi_lcd.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    SPI_LCDexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studySPI_LCDThe use of

 * Hardware resources and pin allocation：
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

 * experimentPhenomenon：
 * 1, Jumper cap connectionLCD_DCandIO_SELback，After successfully downloading the program，LCDmeetingshowexperimentinformation and start refreshing the screen andshowLogoandonerotatecube

 * Things to note：
 * none

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "alientek_logo.h"
#include "demo_show.h"

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    uart_init(0, 115200);   /* serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */

    /* Screen swipe test */
    lcd_clear(BLACK);
    delay(500);
    lcd_clear(RED);
    delay(500);
    lcd_clear(GREEN);
    delay(500);
    lcd_clear(BLUE);
    delay(500);
    lcd_clear(YELLOW);
    delay(500);
    lcd_clear(WHITE);
    delay(500);

    lcd_show_pic(0, 0, 240, 82, ALIENTEK_LOGO);                           /* LCDshowALIENTEKpicture */
    lcd_show_string(10, 100, 200, 32, LCD_FONT_32, "ESP32-S3", RED);      /* LCDshow32size fontESP32S3 */
    lcd_show_string(10, 132, 200, 24, LCD_FONT_24, "TFTLCD TEST", RED);   /* LCDshow24size fontTFTLCD TEST */
    lcd_show_string(10, 156, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED); /* LCDshow16size fontATOM@ALIENTEK */
    delay(500);
}

/**
 * @brief    cyclefunction，Usually put the main body of the program or statements that need to be refreshed continuously.
 * @param    none
 * @retval   none
 */
void loop() 
{
    demo_show_cube();   /* Demonstrate cube 3D rotation */
}