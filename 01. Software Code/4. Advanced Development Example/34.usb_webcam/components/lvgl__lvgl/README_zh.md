<h1 align="center"> LVGL - Light and Versatile Graphics Library</h1>
<h2 align="center"> LVGL - lightweight general-purpose graphics library</h2>

<p align="center">
<img src="https://lvgl.io/assets/images/lvgl_widgets_demo.gif">
</p>
<p align="center">
LVGLis a highly croppable、Low resource usage、Embedded system graphics library with beautiful interface and easy-to-use
</p>

<h4 align="center">
<a href="https://lvgl.io">Official website</a> ·
<a href="https://docs.lvgl.io/">Documentation</a> ·
<a href="https://forum.lvgl.io">Forum</a> ·
<a href="https://lvgl.io/services">Serve</a>  &middot;
<a href="https://docs.lvgl.io/master/examples.html">routine</a>
</h4>

[English](./README.md) | **Chinese** | [Português do Brasil](./README_pt_BR.md)

---

#### Table of contents
- [Overview & Overview](#overview&overview)
- [How to get started](#How to get started)
- [routine](#routine)
- [Service](#service)
- [How to contribute to the community](#How to contribute to the community)

## Overview and Overview
## ## Features
* Rich and powerful modularity[graphics component](https://docs.lvgl.io/master/widgets/index.html)：button (buttons)、chart (charts)、list (lists)、Slider (sliders)、picture (images) wait
* Advanced graphics engine：animation、Anti-aliasing、transparency、smooth scrolling、Layer blending and other effects
* support多种[input device](https://docs.lvgl.io/master/overview/indev.html)：touchscreen、 keyboard、encoder、按键wait
* Support [Multiple display devices](https://docs.lvgl.io/master/overview/display.html)
* Does not depend on a specific hardware platform and can run on any display
* Configuration can be tailored（最Low resource usage：64 kB Flash，16 kB RAM）
* Multi-language support based on UTF-8, such as Chinese, Japanese, Korean, Arabic, etc.
* can pass[kindCSS](https://docs.lvgl.io/master/overview/style.html)way to design、Layout graphical interface（For example：[Flexbox](https://docs.lvgl.io/master/layouts/flex.html)、[Grid](https://docs.lvgl.io/master/layouts/grid.html)）
* Support operating system、external memory、as well as硬件加速（LVGLBuilt-in supportSTM32 DMA2D、SWM341 DMA2D、NXP PXPandVGLite）
* Even if there is only[single buffer(frame buffer)](https://docs.lvgl.io/master/porting/display.html)case，Also ensures silky smooth rendering
* All byCCompleted，and supportC++call
* supportMicropythonprogramming，See：[LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
* support[emulator](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html)simulation，Can be developed without hardware support
* Rich and detailed[routine](https://github.com/lvgl/lvgl/tree/master/examples)
* detailed[document](http://docs.lvgl.io/)as well asAPIrefer to手册，Available online or downloaded asPDFFormat

### Hardware requirements

<table>
  <tr>
    <td> <strong>Requirements</strong> </td>
    <td><strong>Minimum Requirements</strong></td>
    <td><strong>Suggested Requirements</strong></td>
  </tr>
  <tr>
    <td><strong>Architecture</strong></td>
    <td colspan="2">16、32、64microcontroller or microprocessor</td>
  </tr>
  <tr>
    <td> <strong>clock</strong></td>
    <td> &gt; 16 MHz</td>
    <td> &gt; 48 MHz</td>
  </tr>

  <tr>
    <td> <strong>Flash/ROM</strong></td>
    <td> &gt; 64 kB </td>
    <td> &gt; 180 kB</td>
  </tr>

  <tr>
    <td> <strong>Static RAM</strong></td>
    <td> &gt; 16 kB </td>
    <td> &gt; 48 kB</td>
  </tr>

  <tr>
    <td> <strong>Draw buffer</strong></td>
    <td> &gt; 1 &times; <em>hor. res.</em> pixels </td>
    <td> &gt; 1/10screen size </td>
  </tr>

  <tr>
    <td> <strong>Compiler</strong></td>
    <td colspan="2"> C99 or newer </td>
  </tr>
</table>

*Notice：Resource usage and specific hardware platform、compilerwait因素有关，上表中仅给出refer to值*

### Already supported platforms
LVGLIt does not depend on a specific hardware platform，any satisfactionLVGLAll microcontrollers with hardware configuration requirements can runLVGL。
Only some of them are listed below:

- NXP: Kinetis, LPC, iMX, iMX RT
- STM32F1, STM32F3, STM32F4, STM32F7, STM32L4, STM32L5, STM32H7
- Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
- [Linux frame buffer](https://blog.lvgl.io/2018-01-03/linux_fb) (/dev/fb)
- [Raspberry Pi](http://www.vk3erw.com/index.php/16-software/63-raspberry-pi-official-7-touchscreen-and-littlevgl)
- [Espressif ESP32](https://github.com/lvgl/lv_port_esp32)
- [Infineon Aurix](https://github.com/lvgl/lv_port_aurix)
- Nordic NRF52 Bluetooth modules
- Quectel modems
- [SYNWIT SWM341](https://www.synwit.cn/)

LVGLAlso supports：
- [Arduino library](https://docs.lvgl.io/master/get-started/platforms/arduino.html)
- [PlatformIO package](https://platformio.org/lib/show/12440/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/reference/kconfig/CONFIG_LVGL.html)
- [ESP32 component](https://docs.lvgl.io/master/get-started/platforms/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/get-started/os/nuttx.html)
- [RT-Thread RTOS](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/lvgl-docs/introduction)

## How to get started
Please follow the following order to learn LVGL:
1. use[Web page online routines](https://lvgl.io/demos)Come and experienceLVGL（3minute）
2. 阅读document[Introduction](https://docs.lvgl.io/master/intro/index.html)Chapter for a preliminary understandingLVGL（5minute）
3. Let’s read the documentation quickly[Quick overview](https://docs.lvgl.io/master/get-started/quick-overview.html)Chapter to understandLVGLbasic knowledge（15minute）
4. 学习如何use[emulator](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html)来在电脑上simulationLVGL（10minute）
5. Try some hands-on practice[routine](https://github.com/lvgl/lvgl/tree/master/examples)
6. refer to[Migration Guide](https://docs.lvgl.io/master/porting/index.html)try toLVGLPorted to a development board，LVGLSome transplanted ones have also been provided[project](https://github.com/lvgl?q=lv_port_)
7. 仔细阅读document[Overview](https://docs.lvgl.io/master/overview/index.html)章节来更加深入的了解and熟悉LVGL（2-3Hour）
8. 浏览document[components(Widgets)](https://docs.lvgl.io/master/widgets/index.html)Chapter to understand如何use它们
9. If you have any questions you can go toLVGL[forum](http://forum.lvgl.io/)Ask a question
10. 阅读document[How to contribute to the community](https://docs.lvgl.io/master/CONTRIBUTING.html)Chapter to see if you can helpLVGLwhat the community does，to promoteLVGLContinuous improvement of software quality（15minute）

## # routine

For more routines, please see the [examples](https://github.com/lvgl/lvgl/tree/master/examples) folder.

![LVGL button with label example](https://github.com/lvgl/lvgl/raw/master/docs/misc/btn_example.png)

### C
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
lv_obj_set_pos(btn, 10, 10);                                    /*Set its position*/
lv_obj_set_size(btn, 100, 50);                                  /*Set its size*/
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn);                        /*Add a label to the button*/
lv_label_set_text(label, "Button");                             /*Set the labels text*/
lv_obj_center(label);                                           /*Align the label to the center*/
...

void btn_event_cb(lv_event_t * e)
{
  printf("Clicked\n");
}
```
### Micropython
For more information please go to [MicropythonOfficial website](https://docs.lvgl.io/master/get-started/bindings/micropython.html) Query.
```python
def btn_event_cb(e):
  print("Clicked")

# Create a Button and a Label
btn = lv.btn(lv.scr_act())
btn.set_pos(10, 10)
btn.set_size(100, 50)
btn.add_event_cb(btn_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(btn)
label.set_text("Button")
label.center()
```

## Serve
LVGL 责任有限公司成立的目的是为了给用户useLVGLGraphics library provides additional technical support，We are committed to providing the following services：

- Graphic design
- UI design
- Technical consultation and technical support

See https://lvgl.io/services for more information and feel free to contact us if you have any questions.

## # How to contribute to the community
LVGLis an open source project，You are very welcome to participate in community contributions。There are many ways you can improveLVGLContribute your part，including but not limited to：

- Introduce your works or projects based on LVGL design
- write some routines
- Modify and improve documentation
- bug fix

请Seedocument[How to contribute to the community](https://docs.lvgl.io/master/CONTRIBUTING.html)chapter for more information。