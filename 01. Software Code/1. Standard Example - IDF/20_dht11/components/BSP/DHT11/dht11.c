/**
 ****************************************************************************************************
 * @file        dht11.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       DHT11Digital temperature and humidity sensor driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "dht11.h"

/**
 * @brief       ResetDHT11
 * @param       data: To be writtendata
 * @retval      none
 */
void dht11_reset(void)
{
    DHT11_DQ_OUT(0);        /* Pull downDQ */
    vTaskDelay(25);         /* Lower at least 18ms */
    DHT11_DQ_OUT(1);        /* DQ=1 */
    esp_rom_delay_us(30);   /* Host pulls up10~35us */
}

/**
 * @brief       waitDHT11Response
 * @param       none
 * @retval      0, DHT11normal
 *              1, DHT11abnormal/Does not exist
 */
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DHT11_DQ_IN && retry < 100)      /* DHT11 will lower 40~80us */
    {
        retry++;
        esp_rom_delay_us(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DHT11_DQ_IN && retry < 100) /* DHT11 will be pushed up again after being lowered by 40~80us */
        {
            retry++;
            esp_rom_delay_us(1);
        }
        
        if (retry >= 100)
        {
            rval = 1;
        }
    }
    
    return rval;
}

/**
 * @brief Read a bit from DHT11
 * @param None
 * @retval The bit value read: 0 / 1
 */
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DQ_IN && retry < 100)  /* waitTurns low */
    {
        retry++;
        esp_rom_delay_us(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) /* wait to high level */
    {
        retry++;
        esp_rom_delay_us(1);
    }

    esp_rom_delay_us(40);               /* wait40us */

    if (DHT11_DQ_IN)                    /* Return to bit according to pin state */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/**
 * @brief       fromDHT11ReadOne byte
 * @param       none
 * @retval      data：Readdata
 */
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* Loop reading 8-bit data */
    {
        data <<= 1;                 /* High-bit data is output first, Move one left first */
        data |= dht11_read_bit();   /* Read1bitdata */
    }

    return data;
}

/**
 * @brief       fromDHT11Read data once
 * @param       temp: Temperature value(scope:-20~50°)
 * @param       humi: Humidity value(scope:5%~95%)
 * @retval      0, normal.
 *              1, fail
 */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;

    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++) /* Read 40 bits of data */
        {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief       initializationDHT11
 * @param       none
 * @retval      0, normal
 *              1, Does not exist/Nonormal
 */
uint8_t dht11_init(void)
{
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;             /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT_OD;          /* Open-drain mode input and output */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;           /* Enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;      /* Disabled pull-down */
    gpio_init_struct.pin_bit_mask = 1ull << DHT11_DQ_GPIO_PIN;  /* Set the bit mask of the pin */
    gpio_config(&gpio_init_struct);                             /* ConfigurationDHT11Pin */

    dht11_reset();
    return dht11_check();
}