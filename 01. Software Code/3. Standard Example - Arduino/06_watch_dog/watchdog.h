/**
 ****************************************************************************************************
 * @file        watchdog.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       WDG Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20230809
 * First release

 ****************************************************************************************************
 */

#ifndef __WATCHDOG_H
#define __WATCHDOG_H

#include "Arduino.h"
#include "esp_system.h"

extern hw_timer_t *wdg_timer;

/* Watchdog timer definition */
#define WDG_TIMx       1                    /* Timer used to simulate watchdogs */
#define WDG_ISR        tim1_ISR             /* timer interrupt service function */

/* Function declaration */
void wdg_init(uint32_t arr, uint16_t psc);  /* Watchdog initialization function */
void WDG_ISR(void);                         /* Watchdog timer interrupt callback function */

#endif