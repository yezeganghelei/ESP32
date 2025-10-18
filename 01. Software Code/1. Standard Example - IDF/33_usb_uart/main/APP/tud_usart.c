/**
 ****************************************************************************************************
 * @file        tud_usart.c
 * @author      
 * @version     V1.0
 * @date        2023-12-01
 * @brief       USBSimulated serial port code
 * @license     Copyright (c) 2020-2032, 
 ****************************************************************************************************
 * @attention

 ****************************************************************************************************
 */

#include "tud_usart.h"

static const char *TAG = "usb_msc";
static uint8_t buf[CONFIG_TINYUSB_CDC_RX_BUFSIZE + 1];

/**
 * @brief       SDCard simulationUDisk function initialization
 * @param       itf     :Brief settingCDCport
 * @param       event   :CDCevent
 * @retval      none
 */
void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    size_t rx_size = 0;

    /* Read the serial port data */
    esp_err_t ret = tinyusb_cdcacm_read(itf, buf, CONFIG_TINYUSB_CDC_RX_BUFSIZE, &rx_size);

    if (ret == ESP_OK)
    {
        ESP_LOGI(TAG, "Data from channel %d:", itf);
        ESP_LOG_BUFFER_HEXDUMP(TAG, buf, rx_size, ESP_LOG_INFO);
    }
    else
    {
        ESP_LOGE(TAG, "Read error");
    }

    /* Send data */
    tinyusb_cdcacm_write_queue(itf, buf, rx_size);
    tinyusb_cdcacm_write_flush(itf, 0);
}

/**
 * @brief       SDCard simulationUDisk function initialization
 * @param       itf     :Brief settingCDCport
 * @param       event   :CDCevent
 * @retval      none
 */
void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rts = event->line_state_changed_data.rts;
    ESP_LOGI(TAG, "Line state changed on channel %d: DTR:%d, RTS:%d", itf, dtr, rts);
}

/**
 * @brief       SDCard simulationUDisk function initialization
 * @param       none
 * @retval      none
 */
void tud_usb_usart(void)
{
    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .external_phy = false,
        .configuration_descriptor = NULL,
    };
    /* USBEquipment registration */
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,                /* USBequipment */
        .cdc_port = TINYUSB_CDC_ACM_0,              /* CDCport */
        .rx_unread_buf_sz = 64,                     /* ConfigurationRXBuffer size */
        .callback_rx = &tinyusb_cdc_rx_callback,    /* Receive callback function */
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };
    /* USB CDCinitialization */
    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* Register a callback function */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));

    ESP_LOGI(TAG, "USB initialization DONE");
}