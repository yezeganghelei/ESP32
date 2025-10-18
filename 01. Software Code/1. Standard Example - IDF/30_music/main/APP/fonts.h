/**
 ****************************************************************************************************
 * @file        font.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Font library code
 *              supplyfonts_update_fontandfonts_initFor font library updates and initialization
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __FONTS_H
#define __FONTS_H

#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"
#include "ff.h"
#include "lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* Save the first address of the font information
 * occupy33Bytes,The1BytesFor markingFont libraryDoes it exist.Follow-up each8BytesA group,Save the start separatelyaddressandFile size
 */
extern uint32_t FONTINFOADDR;

/* Font library information structuredefinition
 * Used to saveFont libraryBasic information，address，Size, etc.
 */
typedef struct
{
    uint8_t fontok;             /* Font library exists flag, 0XAA, The font library is normal; other, Font library does not exist */
    uint32_t ugbkaddr;          /* unigbkaddress */
    uint32_t ugbksize;          /* unigbkThe size of */
    uint32_t f12addr;           /* gbk12 address */
    uint32_t gbk12size;         /* gbk12 size */
    uint32_t f16addr;           /* gbk16address */
    uint32_t gbk16size;         /* gbk16 size */
    uint32_t f24addr;           /* gbk24address */
    uint32_t gbk24size;         /* gbk24The size of */
} _font_info;

/* Font library information structure */
extern _font_info ftinfo;

/* Function declaration */
esp_err_t fonts_partition_read(void * buffer, uint32_t offset, uint32_t length);                                /* Partition reading */
esp_err_t fonts_partition_write(void * buffer, uint32_t offset, uint32_t length);                               /* Partition writing */
esp_err_t fonts_partition_erase_sector(uint32_t offset);                                                        /* Partition erase */
uint8_t fonts_update_font(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);                  /* Update allFont library */
void fonts_progress_show(uint16_t x, uint16_t y, uint8_t size, uint32_t totsize, uint32_t pos, uint16_t color); /* Show current font update progress */
uint8_t fonts_init(void);                                                                                       /* initializationFont library */

#endif