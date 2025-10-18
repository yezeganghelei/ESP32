/**
 ****************************************************************************************************
 * @file        tud_sd.h
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       SDcard emulationUplate（USB）Code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#ifndef __TUD_SD_H
#define __TUD_SD_H

#include <inttypes.h>
#include "ff.h"
#include "diskio.h"
#include "esp_vfs_fat.h"
#include "tinyusb.h"
#include "esp_idf_version.h"

/* USBcontroller */
typedef struct
{
    uint8_t status;                         /* bit0:0, disconnect; 1, connect */
}__usbdev;

extern __usbdev g_usbdev;                   /* USBcontroller */

/* function declaration */
void tud_usb_sd(void);

#endif