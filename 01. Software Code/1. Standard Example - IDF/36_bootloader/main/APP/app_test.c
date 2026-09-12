/**
 ****************************************************************************************************
 * @file        app_test.c
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

#include "app_test.h"


/**
 * @brief       LED test
 * @param       none
 * @retval      none
 */
int led_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    uint8_t xl_key = 0;
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    printf("\r\n/******************************* 1, LED Test *******************************/\r\n");
    printf("/********************* Note: check whether LED0 and LED1 are blinking!***********************/\r\n");
    printf("/****************************** NEXT: BOOT ;Record: KEY0 *******************************/\r\n");

    while(1)
    {
        key = key_scan(0);
        xl_key = xl9555_key_scan(0);
        LED_TOGGLE();

        if (key == BOOT_PRES)
        {
            LED(1);
            vTaskDelay(500);
            printf("******OK!******\r\n");
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            return TEST_OK;
        }

        if (xl_key == KEY0_PRES)
        {
            LED(1);
            printf("******FAIL!******\r\n");
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            return TEST_FAIL;
        }

        vTaskDelay(100);
    }
}

/**
 * @brief       Key test
 * @param       none
 * @retval      none
 */
int key_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    uint8_t xl_key = 0;
    uint16_t key_status = 0x00;

    printf("\r\n/******************************* 2, Key Test *******************************/\r\n");
    printf("/********************* Press KEY0, KEY1, KEY2, KEY3 and BOOT to exit!***********************/\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    lcd_draw_circle(50,lcd_self.height / 2 + 30,10,WHITE);
    lcd_draw_circle(75,lcd_self.height / 2 + 30,10,WHITE);
    lcd_draw_circle(100,lcd_self.height / 2 + 30,10,WHITE);
    lcd_draw_circle(125,lcd_self.height / 2 + 30,10,WHITE);
    lcd_draw_circle(100,lcd_self.height / 2 + 5,10,WHITE);

    while(1)
    {
        key = key_scan(0);
        xl_key = xl9555_key_scan(0);

        if (key == BOOT_PRES)
        {
            key_status |= 1 << 0;
            lcd_draw_circle(50,lcd_self.height / 2 + 30,10,RED);
            printf("BOOT button pressed\n");
        }

        switch(xl_key)
        {
            case KEY0_PRES:
                key_status |= 1 << 1;
                lcd_draw_circle(125,lcd_self.height / 2 + 30,10,RED);
                printf("KEY0 button pressed\n");
                break;
            case KEY1_PRES:
                key_status |= 1 << 2;
                lcd_draw_circle(100,lcd_self.height / 2 + 30,10,RED);
                printf("KEY1 button pressed\n");
                break;
            case KEY2_PRES:
                key_status |= 1 << 3;
                lcd_draw_circle(75,lcd_self.height / 2 + 30,10,RED);
                printf("KEY2 button pressed\n");
                break;
            case KEY3_PRES:
                key_status |= 1 << 4;
                lcd_draw_circle(100,lcd_self.height / 2 + 5,10,RED);
                printf("KEY3 button pressed\n");
                break;
        }

        if ((key_status & 0x1F) == 0x1F)
        {
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            lcd_fill(30,lcd_self.height / 2 - 10,150,lcd_self.height / 2 + 50,BLACK);
            key_status = 0x00;
            printf("All buttons pressed, exiting normally!\r\n");
            return TEST_OK;
        }

        vTaskDelay(10);
    }
}

/**
 * @brief       Beep test
 * @param       none
 * @retval      none
 */
int beep_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    uint8_t xl_key = 0;
    
    printf("\r\n/******************************* 3, Buzzer Test *******************************/\r\n");
    printf("/**************************** Note: listen to check whether the buzzer is beeping!***********************/\r\n");
    printf("/****************************** NEXT: BOOT ;Record: KEY0 *******************************/\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);

    xl9555_pin_write(BEEP_IO,0);                /* Turn on the buzzer */

    while(1)
    {
        key = key_scan(0);
        xl_key = xl9555_key_scan(0);

        if (key == BOOT_PRES)
        {
            printf("******OK!******\r\n");
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            xl9555_pin_write(BEEP_IO,1);        /* Turn off the buzzer */
            return TEST_OK;
        }

        if (xl_key == KEY0_PRES)
        {
            printf("******FAIL!******\r\n");
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            xl9555_pin_write(BEEP_IO,1);        /* Turn off the buzzer */
            return TEST_FAIL;
        }

        vTaskDelay(10);
    }
}

const uint8_t g_text_buf[] = {"EEPROM"};        /* String array to write to 24C02 */
#define TEXT_SIZE   sizeof(g_text_buf)          /* Length of the TEXT string */
uint8_t datatemp[TEXT_SIZE];

/**
 * @brief       AT24C02 test
 * @param       none
 * @retval      none
 */
int at24cx_test(Test_Typedef * obj)
{
    uint8_t err = 0;
    

    printf("\r\n/******************************* 4，AT24C02 *******************************/\r\n");
    printf("/********************************** NEXT: AUTO ********************************/\r\n\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    err = at24cxx_check();                      /* Detect AT24C02 */

    if (err != 0)
    {
        while (1)                               /* 24C02 not detected */
        {
            printf("24C02 check failed, please check!\n");
            lcd_show_string(40, 30 + obj->label * 16, lcd_self.width, 16, 16, "24C02 failed", WHITE);
            vTaskDelay(10);
        }
    }

    /* Write data */
    at24cxx_write(0, (uint8_t *)g_text_buf, TEXT_SIZE);
    vTaskDelay(100);
    /* Read data */
    at24cxx_read(0, datatemp, TEXT_SIZE);
    vTaskDelay(100);

    /* Check whether the written and read data match */
    if (strcmp((char *)datatemp, (char *)g_text_buf) == 0)
    {
        lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
        printf("******OK!******\r\n");
        return TEST_OK;
    }
    else
    {
        lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", WHITE);
        printf("******FAIL!******\r\n");
        return TEST_FAIL;
    }
}

/**
 * @brief       ADC test
 * @param       none
 * @retval      none
 */
int adc_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    uint8_t xl_key = 0;
    uint16_t adcdata = 0;
    float voltage = 0;
    adc_init();     /* Initialize ADC */
    
    printf("\r\n/******************************* 5, ADC Test *********************************/\r\n");
    printf("/*** Note: connect the jumper at P3, short AIN to RV1, then adjust RV1 and watch the ADC reading change!! ***/\r\n");
    printf("******Confirm whether AIN and RV at P3 are connected******\r\n");
    printf("/*********************************** NEXT: BOOT **********************************/\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    lcd_show_string(10 + 16 * 4, 30 + obj->label * 16, lcd_self.width, 16, 16,"P3:AIN>RV?",RED);      /* Display the integer part of the voltage */

    while(1)
    {
        key = key_scan(0);

        if (key == BOOT_PRES)
        {
            break;
        }

        vTaskDelay(100);
    }
    printf("******Confirm AIN and RV at P3 are connected******\r\n");
    lcd_fill(10 + 16 * 4,30 + obj->label * 16,30 + obj->label * 16 + 16 * 4,30 + obj->label * 16 + 16,BLACK);
    printf("/****************************** NEXT: BOOT ;Record: KEY0 *******************************/\r\n");

    while(1)
    {
        key = key_scan(0);
        xl_key = xl9555_key_scan(0);

        if (key == BOOT_PRES)
        {
            printf("******OK!******\r\n");
            lcd_fill(10 + 16 * 4,30 + obj->label * 16,30 + obj->label * 16 + 16 * 2,30 + obj->label * 16 + 16,BLACK);
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            return TEST_OK;
        }

        if (xl_key == KEY0_PRES)
        {
            printf("******FAIL!******\r\n");
            lcd_fill(10 + 16 * 4,30 + obj->label * 16,30 + obj->label * 16 + 16 * 2,30 + obj->label * 16 + 16,BLACK);
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            return TEST_FAIL;
        }

        adcdata = adc_get_result_average(ADC_ADCX_CHY, 10);
        voltage = (float)adcdata * (3.3 / 4096);                /* Get the computed actual voltage with decimals */
        printf("ADC:%0.2f\r\n",voltage);
        adcdata = voltage;                                      /* Assign the integer part to the adcdata variable */
        lcd_show_xnum(10 + 16 * 4, 30 + obj->label * 16, adcdata, 1, 16, 0, WHITE);      /* Display the integer part of the voltage */
        lcd_show_char(10 + 16 * 4 + 8, 30 + obj->label * 16,'.',16,0, WHITE);
        voltage -= adcdata;                                     /* Remove the integer part already displayed, leaving the fractional part */
        voltage *= 1000;                                        /* Multiply the fractional part by 1000 */
        lcd_show_xnum(10 +  16 * 4 + 16, 30 + obj->label * 16, voltage, 3, 16, 0x80, WHITE);   /* Display the fractional part */

        vTaskDelay(100);
    }
}

/**
 * @brief       AP3216C test
 * @param       none
 * @retval      none
 */
int ap3216c_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    uint8_t xl_key = 0;
    uint16_t ir = 0, als = 0, ps = 0;
    
    printf("\r\n/******************************* 6, Ambient Light Sensor Test **********************************/\r\n");
    printf("/*** Note: you can cover AP3216C and observe ambient light (ALS), proximity (PS) and infrared (IR) parameters ***/\r\n");
    printf("/****************************** NEXT: BOOT ;Record: KEY0 *******************************/\r\n");
    lcd_show_string(10 + 16 + 16 * 3, 30 + obj->label * 16 + 16 * 2, lcd_self.width, 16, 16, ":IR", RED);
    lcd_show_string(10 + 16 + 16 * 3, 30 + obj->label * 16 + 16 * 3, lcd_self.width, 16, 16, ":PS", RED);
    lcd_show_string(10 + 16 + 16 * 3, 30 + obj->label * 16 + 16 * 4, lcd_self.width, 16, 16, ":ALS", RED);
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);

    while(1)
    {
        key = key_scan(0);
        xl_key = xl9555_key_scan(0);

        if (key == BOOT_PRES)
        {
            lcd_fill(10 + 16,30 + obj->label * 16 + 16 * 2,10 + 16 + 16 * 6,30 + obj->label * 16 + 16 * 4 + 16,BLACK);
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            printf("******OK!******\r\n");
            return TEST_OK;
        }

        if (xl_key == KEY0_PRES)
        {
            lcd_fill(10 + 16,30 + obj->label * 16 + 16 * 2,10 + 16 + 16 * 6,30 + obj->label * 16 + 16 * 4 + 16,BLACK);
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            printf("******FAIL!******\r\n");
            return TEST_FAIL;
        }

        ap3216c_read_data(&ir, &ps, &als);                                          /* Read data */
        printf("IR:%d,PS:%d,ALS:%d\r\n",ir, ps, als);
        lcd_show_num(10 + 16, 30 + obj->label * 16 + 16 * 2, ir, 5, 16, WHITE);      /* Display IR data */
        lcd_show_num(10 + 16, 30 + obj->label * 16 + 16 * 3, ps, 5, 16, WHITE);      /* Display PS data */
        lcd_show_num(10 + 16, 30 + obj->label * 16 + 16 * 4, als, 5, 16, WHITE);     /* Display ALS data  */

        vTaskDelay(100);
    }
}

/**
 * @brief       SD test
 * @param       none
 * @retval      none
 */
int sd_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    uint8_t xl_key = 0;
    
    printf("\r\n/******************************* 7, SD Card Test **********************************/\r\n");
    printf("/*** Note: insert the SD card into the TF slot; pressing BOOT marks the test as failed. ***/\r\n");
    printf("/*************************************** NEXT: AUTO **************************************/\r\n\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);

    while(sd_spi_init())
    {
        key = key_scan(0);
        xl_key = xl9555_key_scan(0);

        lcd_show_string(10 + 16 * 4, 30 + obj->label * 16, lcd_self.width, 16, 16, "SDError", RED);

        if (xl_key == KEY0_PRES || key == BOOT_PRES)
        {
            lcd_fill(10 + 16 * 4,30 + obj->label * 16,10 + 16 + 16 * 7,30 + obj->label * 16 + 16 * 2,BLACK);
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            printf("******FAIL!******\r\n");
            return TEST_FAIL;
        }

        vTaskDelay(100);
    }

    printf("******OK!******\r\n");
    lcd_fill(10 + 16 * 4,30 + obj->label * 16,10 + 16 + 16 * 7,30 + obj->label * 16 + 16 * 2,BLACK);
    lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
    return TEST_OK;
}

/**
 * @brief       QMA6100P test
 * @param       none
 * @retval      none
 */
int qma6100p_test(Test_Typedef * obj)
{
    static uint8_t id_data[2];
    
    printf("\r\n/******************************* 8, QMA6100P Test **********************************/\r\n");
    printf("/********************************** NEXT: AUTO ********************************/\r\n\r\n");
    qma6100p_register_read(QMA6100P_REG_CHIP_ID, id_data, 1);    /* Read device ID; normally 0x90 */
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);

    if (id_data[0] == 0x90)
    {
        printf("******OK!******\r\n");
        lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
        return TEST_OK;
    }
    else
    {
        printf("******FAIL!******\r\n");
        lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
        return TEST_FAIL;
    }
}

uint8_t rmt_flag;

/**
 * @brief       RMT test
 * @param       obj: handle
 * @retval      none
 */
int rmt_test(Test_Typedef * obj)
{
    uint8_t key = 0;
    rmt_flag = 0;
    printf("\r\n/******************************* 9, Infrared TX/RX *******************************/\r\n");
    printf("/* Note: connect the jumper at P3, short AIN to RMT, then check whether the infrared received data is 0~255 */\r\n");
    printf("/****************************** NEXT: BOOT *******************************/\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    lcd_show_string(10 + 16 * 4, 30 + obj->label * 16, lcd_self.width, 16, 16,"P3:AIN>RMT?",RED);      /* Display the integer part of the voltage */

    while(1)
    {
        key = key_scan(0);

        if (key == BOOT_PRES)
        {
            break;
        }

        vTaskDelay(100);
    }
    printf("******Confirm AIN and RMT at P3 are connected******\r\n");
    lcd_fill(10 + 16 * 4,30 + obj->label * 16,30 + obj->label * 16 + 16 * 4,30 + obj->label * 16 + 16,BLACK);
    printf("/****************************** NEXT: AUTO ;Record: Long-press KEY0 *******************************/\r\n");
    emission_init();

    if (rmt_flag == 1)
    {
        lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
    }
    else
    {
        lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
    }

    return TEST_OK;
}

extern const uint8_t music_pcm_start[] asm("_binary_canon_pcm_start");
extern const uint8_t music_pcm_end[]   asm("_binary_canon_pcm_end");

/**
 * @brief       ES8388 test
 * @param       none
 * @retval      none
 */
int es8388_test(Test_Typedef * obj)
{
    size_t bytes_write = 0;
    
    printf("\r\n/******************************* 10, MP3 Test *******************************/\r\n");
    printf("/* Note: insert the TF card, create a MUSIC folder on it, and copy the test.wav music file into it */\r\n");
    printf("/********************************** NEXT: AUTO ********************************/\r\n\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    i2s_init();                                         /* Initialize I2S */
    es8388_adda_cfg(1, 0);                              /* Enable DAC, disable ADC */
    es8388_input_cfg(0);                                /* Disable input */
    es8388_output_cfg(1, 1);                            /* Select DAC channel output */
    es8388_hpvol_set(30);                               /* Set headphone volume */
    es8388_spkvol_set(30);                              /* Set speaker volume */
    xl9555_pin_write(SPK_EN_IO,0);                      /* Turn on the speaker */

    while (1)
    {
        bytes_write = i2s_tx_write(music_pcm_start, music_pcm_end - music_pcm_start);

        if (bytes_write > 0)
        {
            printf("******OK!******\r\n");
            i2s_deinit();                               /* Unload I2S */
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            return TEST_OK;
        }
        else
        {
            printf("******FAIL!******\r\n");
            i2s_deinit();                               /* Unload I2S */
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            return TEST_FAIL;
        }

        vTaskDelay(10);
    }

    return TEST_OK;
}


static uint8_t usb_buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];
__usbdev g_usbdev;                              /* USB controller */

/**
 * @brief       Initialize the SD-card USB mass-storage function
 * @param       itf     : configured CDC port
 * @param       event   : CDC event
 * @retval      none
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    size_t rx_size = 0;

    /* Read data from the serial port */
    tinyusb_cdcacm_read(itf, usb_buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);

    /* Send data */
    tinyusb_cdcacm_write_queue(itf, usb_buf, rx_size);
    tinyusb_cdcacm_write_flush(itf, 0);
}

/**
 * @brief       Initialize the SD-card USB mass-storage function
 * @param       itf     : configured CDC port
 * @param       event   : CDC event
 * @retval      none
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    g_usbdev.status |= 0x01;
}

extern esp_err_t tinyusb_cdc_deinit(int itf);

/**
 * @brief       Unload USART
 * @param       none
 * @retval      none
 */
void tud_usb_detint(void)
{
    tusb_stop_task();
    tinyusb_cdc_deinit(TINYUSB_CDC_ACM_0);
}

/**
 * @brief      Initialize USB
 * @param       none
 * @retval      none
 */
void tud_usb_usart(void)
{
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    };
    /* Register the USB device */
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,                /* USB device */
        .cdc_port = TINYUSB_CDC_ACM_0,              /* CDC port */
        .rx_unread_buf_sz = 64,                     /* Configure the RX buffer size */
        .callback_rx = &tinyusb_cdc_rx_callback,    /* Receive callback */
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };
    /* Initialize USB CDC */
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* Register the callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));
}

/**
 * @brief       USB test
 * @param       none
 * @retval      none
 */
int usb_test(Test_Typedef * obj)
{
    uint8_t xl_key = 0;

    printf("/******************************* 11, USB Test *******************************/\r\n");
    printf("/* Note: connect the USB cable to the USB_SLAVE port; the test exits automatically when the PC detects the USB port */\r\n");
    printf("/********************************** NEXT: AUTO ********************************/\r\n\r\n");
    lcd_show_string(10, 30 + obj->label * 16, lcd_self.width, 16, 16, obj->name_test, WHITE);
    tud_usb_usart();                                    /* Initialize USB */

    while(1)
    {
        xl_key = xl9555_key_scan(0);

        if ((g_usbdev.status & 0x0f) == 0x01)
        {
            printf("******OK!******\r\n");
            tud_usb_detint();
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "OK", WHITE);
            return TEST_OK;
        }

        if (xl_key == KEY0_PRES)
        {
            printf("******FAIL!******\r\n");
            lcd_show_string(lcd_self.width - 95, 30 + obj->label * 16, lcd_self.width, 16, 16, "FAIL", RED);
            return TEST_FAIL;
        }

        vTaskDelay(10);
    }
}

/* ALIENTEK logo icon (24x24)
   PCtoLCD2002 font mode: negative code, row-by-row, forward */
const uint8_t APP_ALIENTEK_ICO2424[]=
{
    0x00,0xFF,0x80,0x03,0xFF,0xE0,0x06,0x00,0xF0,0x18,0x70,0x38,0x07,0xFE,0x1C,0x1C,
    0x0F,0x86,0x30,0x01,0xCE,0x4F,0xE0,0x7F,0x3F,0xF8,0x3F,0xFF,0xF0,0x7F,0xFF,0xE0,
    0xFF,0xFF,0xC1,0xFF,0xFF,0x83,0xFF,0xFE,0x07,0xFF,0x7E,0x0F,0xFE,0x7F,0x07,0xF1,
    0x3B,0xC0,0x06,0x30,0xF0,0x3C,0x18,0x3F,0xF0,0x0E,0x07,0x0C,0x0F,0x80,0x30,0x03,
    0xFF,0xE0,0x00,0xFF,0x80,0x00,0x3C,0x00,
};

/**
 * @brief       UI
 * @param       none
 * @retval      none
 */
void app_ui_disp(void)
{
    lcd_clear(BLACK);
    lcd_app_show_mono_icos(5,2,24,24,(uint8_t*)APP_ALIENTEK_ICO2424,YELLOW,BLACK);
    lcd_show_string(24 + 10, 2, lcd_self.width, 24, 24, "ALIENTEK ESP32-S3", WHITE);
    lcd_draw_rectangle(5,25,lcd_self.width - 25,lcd_self.height - 25,WHITE);
    lcd_show_string(5, lcd_self.height - 20, lcd_self.width, 16, 16, "KEY0:Record", WHITE);
    lcd_show_string(lcd_self.width - 95 , lcd_self.height - 20, lcd_self.width, 16, 16, "BOOT:Next", WHITE);
}

const uint8_t g_clear_buf[] = {"000000"};

/**
 * @brief       On-board function test
 * @param       none
 * @retval      none
 */
void func_test(void)
{
    memset(datatemp,0,TEXT_SIZE);

    if (key_scan(0) == BOOT_PRES)
    {
        /* Write data */
        at24cxx_write(0, (uint8_t *)g_clear_buf, TEXT_SIZE);
        vTaskDelay(100);
    }

    /* Read data */
    at24cxx_read(0, datatemp, TEXT_SIZE);
    vTaskDelay(100);

    /* Check whether the written and read data match */
    if (strcmp((char *)datatemp, (char *)g_text_buf) != 0)
    {
        lcd_display_dir(1);                 /* Set landscape mode */
        app_ui_disp();                      /* Display the UI */

        printf("\r\n**************************************\r\n");
        printf("\r\n***ESP32-S3 Full Functional Testing***\r\n");
        printf("\r\n******KEY0:Next******BOOT:Record******\r\n");
        printf("\r\n**************************************\r\n");

        test_create("LED",led_test);
        test_create("KEY",key_test);
        test_create("BEEP",beep_test);
        test_create("AT24",at24cx_test);
        test_create("ADC",adc_test);
        test_create("AP3216C",ap3216c_test);
        test_create("SD",sd_test);
        test_create("QMA6100P",qma6100p_test);
        test_create("RMT",rmt_test);
        test_create("ES8388",es8388_test);
        test_create("USB",usb_test);
        test_handler();
        printf("\r\n/******************************* Test complete *******************************/\r\n");
        lcd_clear(WHITE);               /* Clear the screen */
    }

    lcd_display_dir(0);                 /* Set portrait mode */
}
