/**
 ****************************************************************************************************
 * @file        ds18b20.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       DS18B20Digital temperature sensor drive code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "ds18b20.h"

/**
 * @brief       ResetDS18B20
 * @param       data: Data to be written
 * @retval      none
 */
void ds18b20_reset(void)
{
    DS18B20_DQ_OUT(0);      /* Pull down DQ, reset */
    esp_rom_delay_us(750);  /* Lower at least 750μs */
    DS18B20_DQ_OUT(1);      /* DQ=1, release reset */
    esp_rom_delay_us(15);   /* The host is lifted by 10~35us */
}

/**
 * @brief       waitDS18B20Response
 * @param       none
 * @retval      0, DS18B20normal
 *              1, DS18B20abnormal/Does not exist
 */
uint8_t ds18b20_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DS18B20_DQ_IN && retry < 200)      /* DS18B20Will pull it down200us */
    {
        retry++;
        esp_rom_delay_us(1);
    }

    if (retry >= 200)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DS18B20_DQ_IN && retry < 240) /* DS18B20Pull downWill be raised again later240us */
        {
            retry++;
            esp_rom_delay_us(1);
        }
        if (retry >= 240)
        {
            rval = 1;
        }
    }
    
    return rval;
}

/**
 * @brief       fromDS18B20Read a bit
 * @param       none
 * @retval      Read bit value: 0 / 1
 */
uint8_t ds18b20_read_bit(void)
{
    uint8_t data = 0;
    DS18B20_DQ_OUT(0);
    esp_rom_delay_us(2);
    DS18B20_DQ_OUT(1);
    esp_rom_delay_us(12);

    if (DS18B20_DQ_IN)
    {
        data = 1;
    }

    esp_rom_delay_us(50);
    return data;
}

/**
 * @brief       fromDS18B20Read a byte
 * @param       none
 * @retval      Read data
 */
uint8_t ds18b20_read_byte(void)
{
    uint8_t i, b, data = 0;

    for (i = 0; i < 8; i++)     /* Loop reading8bit data */
    {
        b = ds18b20_read_bit(); /* DS18B20 outputs low-bit data first, and high-bit data then outputs */
        data |= b << i;         /* Fill in every bit of data */
    }

    return data;
}

/**
 * @brief Write a byte to DS18B20
 * @param data: Bytes to be written
 * @retval None
 */
void ds18b20_write_byte(uint8_t data)
{
    uint8_t j;

    for (j = 1; j <= 8; j++)
    {
        if (data & 0x01)
        {
            DS18B20_DQ_OUT(0);  /* Write 0 */
            esp_rom_delay_us(2);
            DS18B20_DQ_OUT(1);  /* Write 1 */
            esp_rom_delay_us(60);
        }
        else
        {
            DS18B20_DQ_OUT(0);  /* Write 0 */
            esp_rom_delay_us(60);
            DS18B20_DQ_OUT(1);  /* Write 1 */
            esp_rom_delay_us(2);
        }

        data >>= 1;             /* Move right，Get high-bit data */
    }
}

/**
 * @brief Start the temperature conversion
 * @param None
 * @retval None
 */
void ds18b20_start(void)
{
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /* Skip rom */
    ds18b20_write_byte(0x44);   /* Convert */
}

/**
 * @brief       initializationDS18B20
 * @param       none
 * @retval      0, normal
 *              1, Does not exist/Nonormal
 */
uint8_t ds18b20_init(void)
{
    gpio_config_t gpio_init_struct;

    gpio_init_struct.intr_type = GPIO_INTR_DISABLE;                 /* Disable pin interrupt */
    gpio_init_struct.mode = GPIO_MODE_INPUT_OUTPUT_OD;              /* Open-drain mode input and output */
    gpio_init_struct.pull_up_en = GPIO_PULLUP_ENABLE;               /* Enable pull-up */
    gpio_init_struct.pull_down_en = GPIO_PULLDOWN_DISABLE;          /* Disabled pull-down */
    gpio_init_struct.pin_bit_mask = 1ull << DS18B20_DQ_GPIO_PIN;    /* Set the bit mask of the pin */
    gpio_config(&gpio_init_struct);                                 /* Configure the DS18B20 pin */

    ds18b20_reset();
    return ds18b20_check();
}

/**
 * @brief       fromds18b20Get the temperature value(Accuracy：0.1C)
 * @param       none
 * @retval      Temperature value （-550~1250）
 * @note        ReturnedTemperature valueZoom in10Double.
 *              When actually used,To divide10It's the actual temperature.
 */
short ds18b20_get_temperature(void)
{
    uint8_t flag = 1;           /* The default temperature is positive */
    uint8_t TL, TH;
    short temp;
    
    ds18b20_start();            /*  ds1820 start convert */
    ds18b20_reset();
    ds18b20_check();
    ds18b20_write_byte(0xcc);   /*  skip rom */
    ds18b20_write_byte(0xbe);   /*  convert */
    TL = ds18b20_read_byte();   /*  LSB */
    TH = ds18b20_read_byte();   /*  MSB */

    if (TH > 7)
    {/* Negative temperature，CheckDS18B20The temperature representation is consistent with the principle of computer storing positive and negative data：
        The positive complement is the data stored in the register itself，Negative complement is the register storage value after bitwise inversion+1
        So we take the actual negative part of it，But the complement of negative numbers is to add one after the inverse，But considering the low level may+1There are carry and code redundancy afterwards，
        We haven't done it yet+1Handling，Pay attention here */
        TH = ~TH;
        TL = ~TL;
        flag = 0;   
    }

    temp = TH;      /* Get the top eight */
    temp <<= 8;
    temp += TL;     /* Get the bottom eight */

    /* Convert to actual temperature */
    if (flag == 0)
    {   /* Convert the temperature to negative temperature, here +1 refers to the previous description */
        temp = (double)(temp+1) * 0.625;
        temp = -temp;   
    }
    else
    {
        temp = (double)temp * 0.625;
    }
    
    return temp;
}