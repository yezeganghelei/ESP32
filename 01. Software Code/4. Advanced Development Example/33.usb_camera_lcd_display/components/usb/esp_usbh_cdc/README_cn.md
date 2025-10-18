* [English version](./README.md)

## USB Host CDC

```
Notice: 当前drive已经迁移到 [master/components/usb/iot_usbh_cdc](https://github.com/espressif/esp-iot-solution/tree/master/components/usb/iot_usbh_cdc),
```

The component实现了简易版本的 USB CDC Host function，仅保留了默认控制传输endpointand批量传输endpoint，Streamlined USB Host enumeration logic，Users only need to bind USB CDC Device endpoint address enables fast initialization，Suitable for customized devices with high startup speed requirements。

The component API The design logic and [ESP-IDF UART drive](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-reference/peripherals/uart.html)The interface is similar，Can be directly replaced UART interface，Implement original code UART-> USB updates。

## ## API usage instructions

1. use `usbh_cdc_driver_install` 来配置and启动内部 USB Task，The most important thing is to specify CDC Bulk endpoint address `bulk_in_ep_addr` and `bulk_out_ep_addr` for communication，And also need to configure transfer Buffer memory size。 Users can also `bulk_in_ep` and `bulk_out_ep` Configure entire descriptor details。

    ```
    /* @brief install usbh cdc driver with bulk endpoint configs and size of internal ringbuffer*/
    static usbh_cdc_config_t config = {
        /* use default endpoint descriptor with user address */
        .bulk_in_ep_addr = EXAMPLE_BULK_IN_EP_ADDR,
        .bulk_out_ep_addr = EXAMPLE_BULK_OUT_EP_ADDR,
        .rx_buffer_size = IN_RINGBUF_SIZE,
        .tx_buffer_size = OUT_RINGBUF_SIZE,
        .conn_callback = usb_connect_callback,
        .disconn_callback = usb_disconnect_callback,
    };

    /* install USB host CDC driver */
    usbh_cdc_driver_install(&config);

    /* Waiting for USB device connected */
    usbh_cdc_wait_connect(portMAX_DELAY);
    ```

2. After successful initialization，The internal state machine handles this automatically USB hot-swappable，Users can also configure hot-swap related callback functions during initialization. `conn_callback` and `disconn_callback`。
3. `usbh_cdc_wait_connect` Can be used for blocking tasks，until USB CDC Device connection or timeout。
4. After successful connection，The host will automatically CDC Device receives USB data to internal `ringbuffer`，Users can poll`usbh_cdc_get_buffered_data_len` to read the size of the cached data or register a receive callback to be notified when the data is ready。Then you can use `usbh_cdc_read_bytes` to read buffered data。
5. `usbh_cdc_write_bytes` Can be used to USB Device sends data。 Data is first written internally `ringbuffer`，then in USB Sent when the bus is idle。
5、`usbh_cdc_driver_delete` Can be completely uninstalled USB drive，Release all resources。

### CDC 多interface支持

This component supports configuring multiple groups of CDC interfaces. Each group of interfaces contains an IN and OUT endpoint. Users use APIs such as `usbh_cdc_itf_read_bytes` and `usbh_cdc_itf_write_bytes` to communicate with the specified interface.

### Examples

* [CDC basic function examples](../../../examples/usb/host/usb_cdc_basic)
* CDC Multiple interface example - [CDC 4G Internet access](../../../examples/usb/host/usb_cdc_4g_module)