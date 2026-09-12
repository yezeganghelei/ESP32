/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file touch.h
 * @author
 * @version V1.0
 * @date 2023-12-1
 * @brief touch screen driver code
 * @note Support capacitive touch screen

 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 * Technical Forum: www.oT_PENedv.com
 
 * Purchase address:oT_PENedv.taobao.com

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#ifndef __TOUCH_H__
#define __TOUCH_H__

#include "ltdc.h"
#include "gt9xxx.h"

#define TP_PRES_DOWN    0x8000  /* The touch screen is pressed */
#define TP_CATH_PRES    0x4000  /* There is a button pressed */
#define CT_MAX_TOUCH    10      /* Number of touch points supported by the capacitive screen, fixed at 5 */

/* touchscreenController */
typedef struct
{
    uint8_t (*init)(void);      /* Initialize the touch screen controller */
    uint8_t (*scan)(uint8_t);   /* scanningtouchscreen.0,Screen Scan;1,Physical coordinates; */
    uint16_t x[CT_MAX_TOUCH];   /* currentcoordinate */
    uint16_t y[CT_MAX_TOUCH];   /* The capacitive screen has at most 10 sets of coordinates; the resistive screen uses only x[0], y[0] to represent the touch coordinates of this scan.
                                 * x[9], y[9] store the coordinates when first pressed.
                                 */

    uint16_t sta;               /* The status of the pen
                                 * b15:Press1/release0;
                                 * b14:0,No button pressed;1,Press with buttons.
                                 * b13~b10:reserve
                                 * b9~b0: The point number of capacitor touchscreenPress (0, It means that no pressing,1Indicates press)
                                 */

    /* 5-point touch screen calibration parameters (the capacitive screen does not require calibration) */
    float xfac;                 /* 5Point calibration methodxDirectional scale factor */
    float yfac;                 /* 5Point calibration methodyDirectional scale factor */
    short xc;                   /* centerXcoordinatephysicsvalue(ADvalue) */
    short yc;                   /* centerYcoordinate physical value(ADvalue) */

    /* New parameters,whentouchscreenIt is necessary when the left, right, up and down are completely reversed.usearrive.
     * b0:0, Vertical screen(Suitable for left and rightXcoordinate,The upper and lowerYCoordinateTP)
     *    1, Horizontal screen(Suitable for left and rightYcoordinate,The upper and lowerXCoordinateTP)
     * b1~6: reserve.
     * b7:0, Resistor screen
     *    1, Capacitor screen
     */
    uint8_t touchtype;
} _m_tp_dev;

extern _m_tp_dev tp_dev;    /* Touch screen controllertouch.cDefinition inside */

/* Function declaration */
uint8_t tp_init(void);      /* touchscreeninitialization */

#endif