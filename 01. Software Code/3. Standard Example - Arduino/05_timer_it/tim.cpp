/**
 ****************************************************************************************************
 * @file        tim.cpp
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       TIM driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "tim.h"
#include "led.h"

hw_timer_t *timer = NULL;

/**
 * @brief       TimerTIMXTimed interrupt initialization function
 * @note
 *              TimerThe clock comes fromAPB,andAPBfor80M
 *              soTimerclock = (80/psc)Mhz, unittimefor 1 / (80 / psc) = x us
 *              TimerOverflow time calculation method: Tout = ((arr + 1) * (psc + 1)) / Ft us
 *              Ft=TimerWorking frequency,unit:Mhz

 * @param       arr: autoreload value
 * @param       psc: clock prescaler
 * @retval      none
 */
void timx_int_init(uint16_t arr, uint16_t psc)
{
    timer = timerBegin(TIMx_INT, psc, true);         /* Initialize the timer0 */

    timerAlarmWrite(timer, arr, true);               /* Set the interrupt time */

    timerAttachInterrupt(timer, &TIMx_ISR, true);    /* ConfigurationTimerInterrupt callback function */

    timerAlarmEnable(timer);                         /* enableTimerinterrupt */
}

/**
 * @brief       TimerTIMXInterrupt callback function
 * @param       none
 * @retval      none
 */
void TIMx_ISR(void)
{
    LED_TOGGLE();
}