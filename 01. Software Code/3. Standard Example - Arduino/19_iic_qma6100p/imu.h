/**
 ****************************************************************************************************
 * @file        imu.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       IMUPose solution code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#ifndef __IMU_H
#define __IMU_H

#include "Arduino.h"

/* Function declaration */
void acc_get_angle(float accl_in[3], float angle[2]);  /* Obtain the Euler angle after attitude solution */

#endif