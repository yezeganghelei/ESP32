/**
 ******************************************************************************
 * @file     16_infrared_transmission.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Infrared transmission experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：Learn infrared transmission

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
 * 5,    R_IN --> ESP32S3 IO
 *         IN --> IO2
 * 6,   R_OUT --> ESP32S3 IO
 *        OUT --> IO8(Jumper cap connection)

 * experimentPhenomenon：
 * 1, Send infrared remote control data according to the program settings，The infrared receiver receives data，existLCDThe key value and the identifier will be displayed

 * Things to note：
 * 1, Need to useJumper cap connectionAINandRMT

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "remote.h"
#include "emission.h"

uint8_t key_value = 0;
uint8_t t = 0;

/**
 * @brief    When the program starts executing，will callsetup()function，Usually used to initialize variables、functions etc.
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
    emission_init();        /* Infrared send initialization */

    lcd_show_string(30,  50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30,  70, 200, 16, LCD_FONT_16, "EMISSION TEST", RED);
    lcd_show_string(30,  90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "TX KEYVAL:", RED);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "RX KEYVAL:", RED);
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    t++;
    if (t == 0)
    {
        t = 1;
    }

    emission_send(t);                                 /* Infrared sending key value */
    lcd_show_num(110, 110, t, 3, LCD_FONT_16, BLUE);  /* Display infrared sending key value */

    key_value = remote_scan();                        /* Infrared receive key value */
    if (key_value)
    {
        lcd_show_num(110, 130, key_value, 3, LCD_FONT_16, BLUE);  /* Display infrared receiving key value */
    }

    LED_TOGGLE();
    delay(200);
}