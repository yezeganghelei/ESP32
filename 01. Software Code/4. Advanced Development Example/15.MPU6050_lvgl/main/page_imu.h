/*
 * @Author: Kevincoooool
 * @Date: 2021-12-09 14:43:07
 * @Description: 
 * @version:  
 * @Filename: Do not Edit
 * @LastEditTime: 2021-12-09 14:43:46
 * @FilePath: \15.MPU6050_lvgl\main\page_imu.h
 */

#ifndef _page_imu_
#define _page_imu_

#ifdef __cplusplus
extern "C" {
#endif
/*********************
* INCLUDES
*********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lvgl.h"
#include "lv_ex_conf.h"
#else
#include "../../lvgl/lvgl.h"
#endif

extern uint8_t imu_en;

void Imu_Task(void *pvParameters);
void page_imu_load(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _TEST_