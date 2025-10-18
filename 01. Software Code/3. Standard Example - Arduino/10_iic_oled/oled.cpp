/**
 ****************************************************************************************************
 * @file        oled.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       OLED driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "oled.h"
#include "xl9555.h"
#include "SSD1306Wire.h"

SSD1306Wire display(OLED_ADDR, OLED_SDA_PIN, OLED_SCL_PIN, GEOMETRY_128_64, I2C_TWO, 800000);   /* definitionOLEDObject(showabnormal,Bundle800000reduce) */

/**
 * @brief       OLEDModuleinitialization
 * @param       none
 * @retval      none
 */
void oled_init(void)
{
    /* initializationOLEDNeed to usearriveofpin */
    pinMode(OLED_D2_PIN, INPUT_PULLUP);           /* OLEDModuleofD2andD1pinNeed to connectcatchtogether，So in order not to interfereD1，set upD2Input mode */
    pinMode(OLED_DC_PIN, OUTPUT);                 /* DCHeel address related */
    digitalWrite(OLED_DC_PIN, 0);                 /* DCpincatchGND，8The bit address is0x78; catchVCC，8The bit address is0x7A */

    xl9555_io_config(OV_RESET, IO_SET_OUTPUT);    /* OLEDModuleresetpinevencatcharriveXL9555DeviceP05pin */
    xl9555_pin_set(OV_RESET, IO_SET_HIGH);        /* The reset pin is pulled high by default */

    /* Hardware reset OLED module */
    OLED_RST(0);
    delay(100);
    OLED_RST(1);
    delay(100);

    display.init();                               /* initializationOLEDModule */
    display.flipScreenVertically();               /* Flip screen verticallyset up */
    display.setFont(ArialMT_Plain_24);            /* set up24Font size */
    display.drawString(0, 0, "ALIENTEK");         /* 24Font sizeshowALIENTEK */
    display.setFont(ArialMT_Plain_16);            /* set up16Font size */    
    display.drawString(0, 24, "0.96' OLED TEST"); /* 16Font sizeshow0.96' OLED TEST */
    display.setFont(ArialMT_Plain_10);            /* set up10Font size */ 
    display.drawString(0, 40, "ATOM 2023/12/1");  /* 10Font sizeshowATOM 2023/12/1 */
    display.drawString(0, 50, "ASCII:");          /* 10Font sizeshowASCII: */
    display.drawString(64, 50, "CODE:");          /* 10Font sizeshowCODE: */
    display.display();                            /* Write cached data to the monitor */
}

/**
 * @brief       OLEDModule display effect
 * @param       none
 * @retval      none
 */
void oled_show_demo(void)
{
    char t = ' ';

    while(1)
    {
        display.setColor(BLACK);                        /* Color set black */
        display.fillRect(35, 50, 10, 15);               /* Clear ASCII display area */
        display.fillRect(104, 50, 20, 15);              /* ClearCODEdisplay area */
        display.setColor(WHITE);                        /* colorset upWhite */
        display.drawString(35, 50, String(t));          /* Display ASCII characters */
        display.drawString(104, 50, String(t - 0));     /* showASCIICharacter code value */
        display.display();                              /* Update display to OLED */

        t++;
        if (t > '~') 
        {
            t = ' ';
        }

        delay(500);
    }
}