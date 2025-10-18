/**
 ******************************************************************************
 * @file     13_adc.ino
 * @author   team()
 * @version  V1.0
 * @date     2023-08-08
 * @brief    ADCexperiment
 * @license  Copyright (c) 2020-2032, 
 ******************************************************************************

 * experimentPurpose：studyADCUse of peripherals

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
 * 4,      RV --> ESP32S3 IO
 *     ADC_IN --> IO8(Jumper cap connection)

 * experimentPhenomenon：
 * 1, existLCDsuperiorshowadjustable rheostatADCvalue and voltage value，Adjusting the rheostat can change the detection voltage

 * Things to note：
 * 1, needJumper cap connectionAINandRV

 ******************************************************************************

 /forum.php

 ******************************************************************************
 */

#include "uart.h"
#include "xl9555.h"
#include "spilcd.h"
#include "adc.h"

uint16_t adc_value = 0;
float adc_vol = 0;

/**
 * @brief    When the program starts executing，Will callsetup()function，Usually used to initialize variables、Functions, etc.
 * @param    none
 * @retval   none
 */
void setup() 
{
    uart_init(0, 115200);   /* Serial port0initialization */
    xl9555_init();          /* IOexpansion chipinitialization */
    lcd_init();             /* LCDinitialization */

    lcd_show_string(30, 50, 200, 16, LCD_FONT_16, "ESP32-S3", RED);
    lcd_show_string(30, 70, 200, 16, LCD_FONT_16, "ADC TEST", RED);
    lcd_show_string(30, 90, 200, 16, LCD_FONT_16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, LCD_FONT_16, "ADC_VAL:", BLUE);
    lcd_show_string(30, 130, 200, 16, LCD_FONT_16, "ADC_VOL:0.000V", BLUE);   /* Firstexistfixed positionshowdecimal point */
}

/**
 * @brief    cyclefunction，Usually the main body of the program or the statement that needs to be refreshed continuously
 * @param    none
 * @retval   none
 */
void loop() 
{
    adc_value = adc_get(ADC_IN_PIN);                                                      /* readGPIO8PinADCvalue */
    lcd_show_xnum(94, 110, adc_value, 5, LCD_FONT_16, NUM_SHOW_NOZERO, BLUE);             /* showADCThe original value after sampling */

    adc_vol = (float)adc_value * 3.3 / 4095;                                              /* Convert to voltage value */
    adc_value = adc_vol;                                                                  /* Assign integer part to the adc_value variable, because adc_value is u16 shaped */
    lcd_show_xnum(94, 130, (uint16_t)adc_value, 1, LCD_FONT_16, NUM_SHOW_NOZERO, BLUE);   /* Integer part of the voltage value，3.1111Word，Here is the display3 */

    adc_vol -= adc_value;                                                                 /* Remove the displayed integer part，Leave a decimal part，for example3.1111-3=0.1111 */
    adc_vol *= 1000;                                                                      /* Multiply the decimal part by1000，For example：0.1111Just convertfor111.1，Equivalent to three decimal places。 */
    lcd_fill(110, 130, 110 + 24, 130 + 16, WHITE);
    lcd_show_xnum(110, 130, adc_vol, 3, LCD_FONT_16, NUM_SHOW_ZERO, BLUE);                /* Display the decimal part (the previous conversion was converted to plastic display), and the 111 is displayed here. */

    delay(500);
}