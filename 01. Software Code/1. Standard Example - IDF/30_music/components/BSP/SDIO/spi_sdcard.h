/**
 ****************************************************************************************************
 * @file        spi_sdcard.h
 * @author      
 * @version     V1.0
 * @date        2023-08-26
 * @brief       SDCard Driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __SPI_SDCARD_H
#define __SPI_SDCARD_H

#include <unistd.h>
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "spi.h"

/* Pin definition */
#define SD_NUM_CS       GPIO_NUM_2
#define MOUNT_POINT     "/0:"

/* Function declaration */
esp_err_t sd_spi_init(void);                                                /* SD card initialization */
void sd_get_fatfs_usage(size_t *out_total_bytes, size_t *out_free_bytes);   /* getSDCard related information */
#endif