[English Version](./README.md)

## # USB Camera LCD Display Demo Description

Note: `uvc_stream` The driver has been migrated to [master/components/usb/usb_stream](https://github.com/espressif/esp-iot-solution/tree/master/components/usb/usb_stream), New version support UVC + UAC Function

This sample program passes `ESP32-S2` or `ESP32-S3` series USB Host function，Realize USB Camera `MJPEG` Data stream reading、本地decoding、screen refresh，Support the following functions：

* Support USB Camera data stream acquisition and analysis
* support JPEG 本地软件decoding
*Support LCD screen display

### Hardware preparation

**Parameters that need to be met for USB camera selection (2021.03):**

  1. Camera兼容 USB1.1 full speed mode
  2. Comes with camera MJPEG compression
  3. The camera supports setting the endpoint `wMaxPacketSize` to `512`
  4. MJPEG supports **320*240** resolution
  5. MJPEG support设置frame率到 15 frame/s

**USB Camera硬件catch线：**
  1. Please use a 5V power supply to power the USB camera VBUS independently. You can also use IO to control VBUS on and off.
  2. USB camera D+ D- data lines should be routed according to conventional differential signal standards.
  3. USB Camera D+ (green line) catch ESP32-S2/S3 GPIO20
  4. USB Camera D- (white line) catch ESP32-S2/S3 GPIO19

## ## Compile sample code

示例代码基于 `esp32-s2-kaluga` Development board writing，可按以下过程直catchcompile烧写：

1. Add to ESP-IDF environment variables，Linux The method is as follows，Please check other platforms [Set up the environment variables](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#step-4-set-up-the-environment-variables)
    ```
    . $HOME/esp/esp-idf/export.sh
    ```
2. Add the ESP-IOT-SOLUTION environment variable. The Linux method is as follows. For other platforms, please refer to [readme](../../../../README_CN.md)
    ```
    export IOT_SOLUTION_PATH=$HOME/esp/esp-iot-solution
    ```
3. According to the camera configuration descriptor，[Modify camera configuration items](../../../../components/usb/uvc_stream/README.md)
4. Set the compilation target to `esp32s2` or `esp32s3`
    ```
    idf.py set-target esp32s2
    ```
5. compile、download、View output
    ```
    idf.py build flash monitor
    ```

### Instructions for use

1. Notice LCD 屏幕和Cameracatch口catch线即可
2. The screen directly outputs the camera image by default
3. Users can enable `boot animation` Enable boot animation，verify LCD Whether the screen can be successfully lit

## ## Performance parameters

**ESP32-S2 240Mhz**：

| Typical resolution | USB typical transfer frame rate | JPEG maximum decoding + display frame rate* |
| :-----: | :--------------: | :----------------------: |
| 320*240 |        15        |           ~12            |
| 160*120 |        30        |           ~28            |

1. *JPEG decoding+ 显示frame率随 CPU Load fluctuation
2. Other resolutions are subject to actual testing.，or按照decoding时间和分辨率正相关估算