/**
 ****************************************************************************************************
 * @file        image.c
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-12-01
 * @brief       Image library code (provides image_update_image and images_init for updating and initializing the image library)
 * @license     Copyright (c) 2020-2032, Guangzhou Xingyi Electronic Technology Co., Ltd.
 ****************************************************************************************************
 * @attention
 *
 * Platform: ALIENTEK ESP32-S3 development board
 * Online video: www.yuanzige.com
 * Technical forum: www.openedv.com
 * Company website: www.alientek.com
 * Purchase: openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __IMAGE_H
#define __IMAGE_H

#include "esp_partition.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"

/* Font information start address
 * Occupies 33 bytes; the first byte marks whether the font library exists. Each following group of 8 bytes stores a start address and file size.
 */
extern uint32_t IMAGEINFOADDR;

/* Font library information structure definition
 * Stores basic font library information, addresses, sizes, etc.
 */
typedef struct
{
    uint8_t imageok;             /* Image library presence flag: 0XBB = normal; other = missing */
    
    uint32_t lvgl_camera_addr;
    uint32_t lvgl_camera_size;
  
    uint32_t lvgl_file_addr;
    uint32_t lvgl_file_size;
    
    uint32_t lvgl_video_addr;
    uint32_t lvgl_video_size;
    
    uint32_t lvgl_setting_addr;
    uint32_t lvgl_setting_size;
    
    uint32_t lvgl_weather_addr;
    uint32_t lvgl_weather_size;
    
    uint32_t lvgl_measure_addr;
    uint32_t lvgl_measure_size;

    uint32_t lvgl_photo_addr;
    uint32_t lvgl_photo_size;
    
    uint32_t lvgl_music_addr;
    uint32_t lvgl_music_size;

    uint32_t lvgl_calendar_addr;
    uint32_t lvgl_calendar_size;

    uint32_t lvgl_background_addr;
    uint32_t lvgl_background_size;
} _image_info;

/* Font library information structure */
extern _image_info g_ftinfo;

/* Function declarations */
uint8_t images_init(void);                                                                          /* Initialize images */
uint8_t images_update_image(uint16_t x, uint16_t y, uint8_t size, uint8_t *src, uint16_t color);    /* Update image files */
esp_err_t images_partition_write(void *buffer, uint32_t offset, uint32_t length);                   /* Write data to the partition table */
esp_err_t images_partition_read(void *buffer, uint32_t offset, uint32_t length);                    /* Read data from the partition table */
esp_err_t images_partition_erase_sector(uint32_t offset);                                           /* Erase a sector */

#endif
