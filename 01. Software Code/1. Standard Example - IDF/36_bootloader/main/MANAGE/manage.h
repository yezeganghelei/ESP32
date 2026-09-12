/**
 ****************************************************************************************************
 * @file        manage.h
 * @author      ALIENTEK Team (ALIENTEK)
 * @version     V1.0
 * @date        2023-08-01
 * @brief       Management
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#ifndef __MANAGE_H
#define __MANAGE_H

#include "list.h"
#include <stdint.h>
#include <stddef.h>
#include "esp_timer.h"


/* Test experiment status */
enum STATE
{
    TEST_OK,
    TEST_FAIL
};

/* Menu structure */
typedef struct Test
{
    atk_list_node_t test_list_node;     /* Node of the parent list */
    char *name_test;                    /* Experiment name */
    uint8_t label;                      /* Label */
    int (*Function)(void * widget);     /* Test function */
}Test_Typedef;

extern uint16_t test_status;            /* Exported variable */

/* Function declarations */
Test_Typedef *test_create(char name[],int (*pfunc)(Test_Typedef * obj));
void test_handler(void);

#endif
