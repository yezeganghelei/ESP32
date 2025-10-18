# Quick Start [[English]](../en/get_started.md)

This article describes how to build ESP-DL environment。You can use[Espressif](https://www.espressif.com/zh-hans/products/devkits)or any other supplier designed ESP Development board。

## Get ESP-IDF

ESP-DL The operation depends on ESP-IDF。related ESP-IDF Detailed installation steps，Please check [ESP-IDF Programming Guide](https://idf.espressif.com/zh-cn/index.html)。

## Get ESP-DL and run the example

1. Use the following command to download ESP-DL：

    ```shell
    git clone https://github.com/espressif/esp-dl.git
    ```

2. Open terminal，Enter [ESP-DL/tutorial](../../tutorial/) folder：

    ```shell
    cd ~/esp-dl/tutorial
    ```

    或是Enter [ESP-DL/examples](../../examples) folder下的其他示例项目。

3. Use the following command to set up the target chip:

    ```shell
    idf.py set-target [SoC]
    ```
    Replace [SoC] with your target chip, such as esp32, esp32s2, esp32s3.

    > Notice ESP32-C3 Only applicable if no PSRAM Application。

4. Burn firmware，Print results：

    ```shell
    idf.py flash monitor
    ```
    
    If in the second step you enter [ESP-DL/tutorial](../../tutorial/) folder，

    - Your target chip is ESP32, then
      
      ```shell
      MNIST::forward: 37294 μs
      Prediction Result: 9
      ```

    - Your target chip is ESP32-S3，but

      ```shell
      MNIST::forward: 6103 μs
      Prediction Result: 9
      ```

## # ESP-DL used as component

ESP-DL is a repository containing various deep learning APIs. We recommend using ESP-DL as a component in other projects.

for example，ESP-DL can be used as [ESP-WHO](https://github.com/espressif/esp-who) Repository submodules，Just change ESP-DL join to [esp-who/components/](https://github.com/espressif/esp-who/tree/master/components) Directory is enough。