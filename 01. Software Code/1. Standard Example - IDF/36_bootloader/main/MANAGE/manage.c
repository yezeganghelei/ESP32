/**
 ****************************************************************************************************
 * @file        manage.c
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

#include "manage.h"


atk_list_node_t test_list_head = ATK_LIST_INIT(test_list_head);
uint16_t test_status = 0x00;  /* Record test status */

/**
 * @brief       Create a test item
 * @param       pfunc: Test function entry
 * @retval      Test item control block
 */
Test_Typedef *test_create(char name[],int (*pfunc)(Test_Typedef * obj))
{
    static int i = 0;
    Test_Typedef * obj = NULL;

    obj = malloc(sizeof(Test_Typedef));                     /* Allocate memory for the test control block */
    obj->label = i ++;                                      /* Assign a label to each control block */
    obj->Function = pfunc;                                  /* Point to the test function */
    obj->name_test = name;                                  /* Experiment name */
    atk_list_add_tail(&test_list_head,&obj->test_list_node);/* Insert the object at the tail of the list */
    return obj;                                             /* Return the test function control block */
}

/**

 * @brief       Run the test items
 * @param       None
 * @retval      None
 */
void test_handler(void)
{
    int status = 0;
    Test_Typedef *data;
    Test_Typedef *data_temp;
    data = (Test_Typedef *)malloc(sizeof(Test_Typedef));                /* Allocate control block memory */

    /* Traverse the test item list */
    atk_list_for_each_entry_safe(data, data_temp, &test_list_head, Test_Typedef, test_list_node)
    {
        status = data->Function(data);                  /* Execute the test code */

        if (status == TEST_FAIL)
        {
            test_status |= (1 << data->label);          /* Record the test failure */
        }
    }

    printf("0x%x\r\n",test_status);
}