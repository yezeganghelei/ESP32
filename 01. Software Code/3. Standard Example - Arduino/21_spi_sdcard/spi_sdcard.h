/**
 ****************************************************************************************************
 * @file        spi_sdcard.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SDCARD driver code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 * Modification instructions
 * V1.0 20231201
 * first release

 ****************************************************************************************************
 */

#ifndef __SPI_SDCARD_H
#define __SPI_SDCARD_H

#include "Arduino.h"

/* Pin definition */
#define SD_CS_PIN         2   
#define SD_MISO_PIN       13 
#define SD_MOSI_PIN       11
#define SD_SCK_PIN        12

/* function declaration */
uint8_t sdcard_init(void);    /* initializationSDCard */
void sd_test(void);           /* SDcard test function */

#endif