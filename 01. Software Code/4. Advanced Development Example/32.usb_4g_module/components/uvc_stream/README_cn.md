## # UVC Stream component description

`UVC Stream` is based on `UVC` protocol developed USB Camera驱动，用户可use ESP32-S2/ESP32-S3 as USB Host，Request and receive continuously USB Camera `MJPEG` image frame。Cooperate `ESP-IOT-Solution` Image decoding or network transmission components，Screen display or IPC and other applications。

Users can simply API Interface controls video stream startup、pause、Restart、and stop operation。By registering a callback function，可在得到完整image frame时，Application layer processing of image data。

### Development environment preparation

1. build ESP-IDF `master` branch development environment：[installation-step-by-step](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html#installation-step-by-step)
2. build `ESP-IOT-Solution` environment：[Setup ESP-IOT-Solution Environment](../../../README.md)

### Hardware preparation

* Development board

  1. The chips that have been adapted in this warehouse are `ESP32-S2` 、`ESP32-S3`
  2. 资源use情况详见示例程序说明文件

* USB Camera

  1. The camera must be compatible `USB1.1` full speed mode
  2. Camera需要自带 `MJPEG` compression
  3. Camera支持设置接口 `Max Packet Size` for `512`
  4. image data stream USB The total transmission bandwidth should be less than `4 Mbps` （500 KB/s）
  5. For details on resolution and other requirements, please refer to the sample program description file.

### UVC Stream API use说明

1. 用户需提前了解待适配Camera`configuration descriptor`Detailed parameters of，Linux 用户可use `lsusb -v` Check， Fill in accordingly `uvc_config_t` configuration items，The corresponding relationship between parameters is as follows：

```
    uvc_config_t uvc_config = {
        .dev_speed = USB_SPEED_FULL, //fixed to USB_SPEED_FULL
        .configuration = 1, //configuration descriptor编号，Generally 1
        .format_index = 1, // bFormatIndex corresponding to MJPEG, for example, 1
        .frame_width = 320, // MJPEG horizontal pixels，For example 320
        .frame_height = 240, // MJPEG vertical pixels，For example 240
        .frame_index = 1, //MJPEG 320*240 Corresponding bFrameIndex, For examplefor 1
        .frame_interval = 666666, //Optional frame rate dwFrameInterval，For example 15fps
        .interface = 1, // Optional video streaming interface bInterfaceNumber，Generally 1
        .interface_alt = 1, // Interface options bAlternateSetting, For example 1 Corresponding endpoint MPS maximum support 512
        .isoc_ep_addr = 0x81, // Interface optionsCorresponding bEndpointAddress, For examplefor 0x81
        .isoc_ep_mps = 512, // Interface options的确定的 MPS， For examplefor 512
        .xfer_buffer_size = 32*1024, //Single frame image size，Need to be determined based on actual testing，320*240 Generally less than 35KB
        .xfer_buffer_a = pointer_buffer_a, // Already applied for buffer pointer
        .xfer_buffer_b = pointer_buffer_b, // Already applied for buffer pointer
        .frame_buffer_size = 32*1024, //Single frame image size needs to be determined based on actual testing
        .frame_buffer = pointer_frame_buffer, // Already applied buffer pointer
    };
```

2. use `uvc_streaming_config` General 1 step by step determined `uvc_config_t` Parameters passed to the driver；
3. use `uvc_streaming_start` With the first 2 Start the video stream according to the parameters passed in.，If parameters are negotiated，Camera将持续输出数据流。该驱动将在检测到完整的image frame时，Call the user's callback function to decode the image、Refresh the screen、Transmission and other operations；
4. If no. 3 Step execution error，Please lower the resolution or frame rate target，Back to Chapter 1 Step modification parameters；
5. Use `uvc_streaming_suspend` to pause the camera video stream;
6. use `uvc_streaming_resume` 可RestartCamera视频流；
7. use `uvc_streaming_stop` Stop video streaming，USB Resources will be fully released。

### USB Camera示例程序

1. [USB Camera + Wi-Fi Picture transmission](../../../examples/usb/host/usb_camera_wifi_transfer)
2. [USB Camera + LCD local display](../../../examples/usb/host/usb_camera_lcd_display)
3. [USB Camera + SD Card Storage](../../../examples/usb/host/usb_camera_sd_card)