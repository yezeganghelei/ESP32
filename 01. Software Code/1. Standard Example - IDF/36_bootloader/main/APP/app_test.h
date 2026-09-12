/**
 ****************************************************************************************************
 * @file        app_test.h
 * @author      ALIENTEK team
 * @version     V1.0
 * @date        2023-08-01
 * @brief       APP function test
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

#ifndef __APP_TEST_H
#define __APP_TEST_H

#include "xl9555.h"
#include "manage.h"
#include "lcd.h"
#include "led.h"
#include "key.h"
#include "24cxx.h"
#include "adc1.h"
#include "ap3216c.h"
#include "spi_sdcard.h"
#include "qma6100p.h"
#include "es8388.h"
#include "i2s.h"
#include <inttypes.h>
#include "tinyusb.h"
#include "tusb_cdc_acm.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "tusb_tasks.h"
#include "emission.h"
#include "manage.h"


/* USB controller */
typedef struct
{
    uint8_t status;                         /* bit0: 0 disconnected; 1 connected */
}__usbdev;

/* Function declarations */
int led_test(Test_Typedef * obj);
int key_test(Test_Typedef * obj);
int beep_test(Test_Typedef * obj);
int at24cx_test(Test_Typedef * obj);
int adc_test(Test_Typedef * obj);
int ap3216c_test(Test_Typedef * obj);
int sd_test(Test_Typedef * obj);
int qma6100p_test(Test_Typedef * obj);
int es8388_test(Test_Typedef * obj);
int usb_test(Test_Typedef * obj);
int rmt_test(Test_Typedef * obj);
void func_test(void);
#endif