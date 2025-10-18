/**
 ****************************************************************************************************
 * @file        watchdog.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       WDG Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#include "watchdog.h"

hw_timer_t *wdg_timer = NULL;

/**
 * @brief       Initialize the watchdog
 * @note
 *              The timer clock comes fromAPB,andAPBfor80M
 *              So the timer clock = (80/psc)Mhz, unittimefor 1 / (80 / psc) = x us
 *              Timer overflow time calculation method: Tout = ((arr + 1) * (psc + 1)) / Ft us
 *              Ft=Timer operating frequency,unit:Mhz

 * @param       arr: Automatic reinstall value
 * @param       psc: clock prescaler
 * @retval      none
 */
void wdg_init(uint32_t arr, uint16_t psc)
{
    wdg_timer = timerBegin(WDG_TIMx, psc, true);         /* Initialize the timer1 */

    timerAlarmWrite(wdg_timer, arr, true);               /* Set the interrupt time */

    timerAttachInterrupt(wdg_timer, &WDG_ISR, true);     /* Configure the timer interrupt callback function */

    timerAlarmEnable(wdg_timer);                         /* Enable timer interrupt */
}

/**
 * @brief       Watchdog timer interrupt callback function
 * @param       none
 * @retval      none
 */
void WDG_ISR(void)
{
    ets_printf("reboot\n");
    esp_restart();
}