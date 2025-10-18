/**
 ******************************************************************************
 * @file     18_dht11.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-12-01
 * @brief    Temperature and humidity sensor experiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studyTemperature and humidity sensorThe use of

 * Hardware resources and pin allocation：
 * 1,     LED --> ESP32S3 IO
 *        LED --> IO1
 * 2,   UART0 --> ESP32S3 IO
 *       TXD0 --> IO43
 *       RXD0 --> IO44
 * 3,  XL9555 --> ESP32S3 IO
 *        SCL --> IO42
 *        SDA --> IO41
 * 4, SPI_LCD --> ESP32S3 IO / XL9555
 *         CS --> IO21
 *        SCK --> IO12
 *        SDA --> IO11
 *         DC --> IO40(Jumper cap connection)
 *        PWR --> XL9555_P13
 *        RST --> XL9555_P12
 * 5,   DHT11 --> ESP32S3 IO
 *         DQ --> IO0(Jumper cap connection)

 * experimentPhenomenon：
 * 1, LCDWill displayDHT11Temperature and humidity data collected by temperature and humidity sensors

 * Things to note：
 * 1, Need to useJumper cap connection1WIRE_DQandIO0
 * 2, IO0andBOOTThe buttons share oneIOoral，Need time-sharing reuse

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */ 

#include "led.h"
#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "dht11.h"

uint8_t temperature;    /* Temperature value */
uint8_t humidity;       /* Humidity value */
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
    dht11_init();           /* DHT11initialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "DHT11 TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "Temp:  C", BLUE);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "Humi:  %", BLUE);
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    if (t % 10 == 0)  /* Every100msRead once */
    {
        dht11_read_data(&temperature, &humidity);                       /* Read temperatureHumidity value */
        lcd_show_num(30 + 40, 110, temperature, 2, LCD_FONT_16, BLUE);  /* Display temperature */
        lcd_show_num(30 + 40, 130, humidity, 2, LCD_FONT_16, BLUE);     /* Display humidity */
    }

    delay(10);
    t++;

    if (t == 20)
    {
        t = 0;
        LED_TOGGLE();     /* LED0 flashes */
    }
}