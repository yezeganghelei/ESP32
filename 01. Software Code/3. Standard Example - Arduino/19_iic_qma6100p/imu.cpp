/**
 ****************************************************************************************************
 * @file        imu.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       IMUAttitude calculation code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * First release

 ****************************************************************************************************
 */

#include "imu.h"
#include <math.h>

#define M_PI		  	(3.14159265358979323846f)
#define RAD_TO_DEG	(180.0f / M_PI) 	

/**
 * @brief       Obtain the attitude solutionEuler angle
 * @param       accl_in  : 3Axis acceleration data
 * @param       angle  : Pitch angle、roll angle
 * @retval      return value : Euler angle
 */
void acc_get_angle(float accl_in[3], float angle[2])
{   
    float accl_data[3];
    float acc_normal, pitch, roll;

    acc_normal = sqrtf(accl_in[0] * accl_in[0] + accl_in[1] * accl_in[1] + accl_in[2] * accl_in[2]);
    
    accl_data[0] = accl_in[0] / acc_normal;
    accl_data[1] = accl_in[1] / acc_normal;
    accl_data[2] = accl_in[2] / acc_normal;

    pitch = -atan2f(accl_in[0], accl_in[2]);
    angle[0] = pitch * RAD_TO_DEG;  

    acc_normal = sqrtf(accl_data[0] * accl_data[0] + accl_data[1] * accl_data[1] + accl_data[2] * accl_data[2]);
    roll = asinf((accl_data[1] / acc_normal));
    angle[1] = roll * RAD_TO_DEG; 
    //Serial.printf("g_pitch:%lf g_roll:%lf \r\n", angle[0], angle[1]);
}