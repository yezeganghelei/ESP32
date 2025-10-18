/**
 ******************************************************************************************************
 * @file sensor.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief Internal temperature sensor driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************
 * @attention

 ******************************************************************************************************
 */

#include "sensor.h"

esp_err_t rev_flag;
temperature_sensor_handle_t temp_handle = NULL; /* Temperature sensor handle */

/**
 * @brief       Initialize internal temperature sensor
 * @param       none
 * @retval      none
 */
void temperature_sensor_init(void)
{
    temperature_sensor_config_t temp_sensor;
    
    temp_sensor.range_min = SENSOR_RANGE_MIN;   /* The minimum temperature value to be tested */
    temp_sensor.range_max = SENSOR_RANGE_MAX;   /* The maximum temperature to be tested */

    rev_flag |= temperature_sensor_install(&temp_sensor, &temp_handle);
    ESP_ERROR_CHECK(rev_flag);
}

/**
 * @brief       Get the internal temperature sensor temperature value
 * @param       none
 * @retval      Returns the internal temperature value
 */
float sensor_get_temperature(void)
{
    float temp;

    /* Enable temperature sensor */
    rev_flag |= temperature_sensor_enable(temp_handle);

    /* Obtain transmitted sensor data */ 
    rev_flag |= temperature_sensor_get_celsius(temp_handle, &temp);

    /* After the temperature sensor is used，Disable temperature sensor，Save power consumption */
    rev_flag |= temperature_sensor_disable(temp_handle);
    ESP_ERROR_CHECK(rev_flag);

    return temp;
}