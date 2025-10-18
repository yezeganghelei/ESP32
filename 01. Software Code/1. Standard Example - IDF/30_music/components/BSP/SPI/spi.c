/**
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @file spi.c
 * @author
 * @version V1.0
 * @date 2023-08-26
 * @brief SPI driver code
 * @license Copyright (c) 2020-2032,
 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 * @attention

 ******************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************************
 */

#include "spi.h"

/**
 * @brief       initializationSPI
 * @param       none
 * @retval      none
 */
void spi2_init(void)
{
    esp_err_t ret = 0;
    spi_bus_config_t spi_bus_conf = {0};

    /* SPIBus configuration */
    spi_bus_conf.miso_io_num = SPI_MISO_GPIO_PIN;                               /* SPI_MISO pin */
    spi_bus_conf.mosi_io_num = SPI_MOSI_GPIO_PIN;                               /* SPI_MOSI pin */
    spi_bus_conf.sclk_io_num = SPI_CLK_GPIO_PIN;                                /* SPI_SCLK pin */
    spi_bus_conf.quadwp_io_num = -1;                                            /* SPIWrite protection signal pin，This pin is not enabled */
    spi_bus_conf.quadhd_io_num = -1;                                            /* SPIKeep signal pin，This pin is not enabled */
    spi_bus_conf.max_transfer_sz = 320 * 240 * 2;                               /* Configure the maximum transfer size，Single in bytesBit */
    
    /* initializationSPIbus */
    ret = spi_bus_initialize(SPI2_HOST, &spi_bus_conf, SPI_DMA_CH_AUTO);        /* SPI bus initialization */
    ESP_ERROR_CHECK(ret);                                                       /* Verify parameter values */
}

/**
 * @brief       SPISend command
 * @param       handle : SPIHandle
 * @param       cmd    : wantSend command
 * @retval      none
 */
void spi2_write_cmd(spi_device_handle_t handle, uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t = {0};

    t.length = 8;                                       /* Number of bits to be transmitted One byte 8Bit */
    t.tx_buffer = &cmd;                                 /* Fill in the command */
    ret = spi_device_polling_transmit(handle, &t);      /* Start transmission */
    ESP_ERROR_CHECK(ret);                               /* There will generally be no problem */
}

/**
 * @brief       SPISend data
 * @param       handle : SPIHandle
 * @param       data   : Data to be sent
 * @param       len    : Data to be sentlength 
 * @retval      none
 */
void spi2_write_data(spi_device_handle_t handle, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t = {0};

    if (len == 0)
    {
        return;                                     /* Length is 0 No data to be transferred */
    }

    t.length = len * 8;                             /* Number of bits to be transmitted One byte 8Bit */
    t.tx_buffer = data;                             /* Fill in the command */
    ret = spi_device_polling_transmit(handle, &t);  /* Start transmission */
    ESP_ERROR_CHECK(ret);                           /* There will generally be no problem */
}

/**
 * @brief       SPIProcessing data
 * @param       handle       : SPIHandle
 * @param       data         : Data to be sent 
 * @retval      t.rx_data[0] : Received data
 */
uint8_t spi2_transfer_byte(spi_device_handle_t handle, uint8_t data)
{
    spi_transaction_t t;

    memset(&t, 0, sizeof(t));

    t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    t.length = 8;
    t.tx_data[0] = data;
    spi_device_transmit(handle, &t);

    return t.rx_data[0];
}