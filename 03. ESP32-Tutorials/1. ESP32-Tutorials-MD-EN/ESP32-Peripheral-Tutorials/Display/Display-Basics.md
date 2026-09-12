This page overview

# Display Basics and Interfaces

NoneRegardless of which driverClasstype/modelofScreen, pixels,Color depth、frame buffer,Display orientation and coordinate systemthese basic concepts are allGeneralof；And the screenAndbetween main controllersofData path (i.e. displayInterface）then directly determines the refresh bandwidthAndPinOverhead. This chapter introduces these two parts, asIsSubsequent screen chaptersofFoundation. The followingPress ESP32 ecosystemInconstantSeeSolution explanation, each entry withDriver IC AndModule documentationPrevails.

## 1. Common concepts of display

### 1.1 pixelAndResolution

on the screenofscreen/imageByeach pixel (Pixel）composed of.ResolutionDescribe the screen horizontallyAndvertical eachYesHow many pixels,For example 240×320 indicates horizontal 240 units, vertical 320 pixels. DriverScreen'sessence, isIsEachpixel specifies to displayofColor。

### 1.2 Color depth

Color Depth refers to how many bits are used to describe the color of each pixel, directly determining the data size per pixel:

|Color depthData per pixelcan represent colorsTypical scenarios
|1bpp (monochrome)1 bitBlack / Whitemonochrome OLED、RLCD、BlackWhitee-ink screen
|RGB56516 bit (2 bytes)65536 colorsMainstream format for embedded color screens
|RGB88824 bit (3 bytes)Approx. 16.77 million colorsScenes with high color requirements

RGB565 Use 5 bit representsRedcolor,6 bit representsGreencolor,5 bit representsBluecolor, inColoreffectAndBalance between data volumes, is ESP32 Most commonly used for color screen developmentUseofformat.

RGB565 ofbit allocation

embedded LCD constantUse RGB565，Is becauseIsa pixel isOKoccupy 16 bit (2 bytes), than 24 bit RGB888 Takes up one-third lessofvideo memory/GRAMAndtransfer bandwidth. For SPI、I80 etc.InterfaceIn terms of, transmit less 1 bytes can usually directly reduceLowRefresh pressure, so manySmall and medium-sized LCD ExampleAndgraphicsLibrarydefaultUse RGB565。
GreenColor component accounts for 6 bits, becauseIsThe human eye toGreencolorBrightnessChangeMore sensitive, to G Allocate more 1 bits can allowBrightnessFiner transitions. This way the total number of bits remains 16 bits, while alsoMorelevels left for visualBrightnesssignificant impactofGreenColor channel.

---

By“Resolution × Color depth”Can calculate a complete frameofData volume:

|Screen exampleResolutionColor depthOne frame data size
|0.96 inch monochrome OLED128×641bpp1 KB
|1.28 inch round LCD240×240RGB565112.5 KB
|2.4 inch LCD240×320RGB565150 KB
|4.3 inch RGB screen800×480RGB565750 KB

In this Tutorials `sendBuffer()`、`sendBuffer()` Useas binary capacity unit:`sendBuffer()`，`sendBuffer()`. This is a convention consistent with the common notation in ESP32 and related product documentation; according to IEC 80000-13, the strict binary unit symbols are `sendBuffer()` And `sendBuffer()`。transmission rateIn `sendBuffer()`、`sendBuffer()` PressDecimal count, i.e. `sendBuffer()`。

The frame data size directly affects two key factors: how much data needs to be transmitted to refresh one frame (affecting frame rate), and how much memory is needed to buffer one frame (affecting the memory scheme).

### 1.3 Frame Buffer and Display Memory (GRAM)

For those requiring continuous scanning and refreshofScreen, current displayofAll pixel data needs to be savedIna piece ofMemoryIn，this blockMemorycalledIsframe buffer (Frame Buffer）。It may be located in two positions:

- **Screen side**:most SPI / I80 InterfaceofScreen, itsDriver IC Internally integrated with display memory (GRAM，Graphics RAM，graphics random accessStorageDevice). The controller sends pixelsData write GRAM after,Driver IC responsible for continuously refreshing the panel itself. The main controllerNoneneed to save the entire frame,Memorylow pressure, when the screen is not updating, the main controllerNoneneed to perform any operation.
- **Controller side**: RGB interface screens do not have GRAM; the main controller must maintain a complete frame buffer in its own memory and continuously output data to the screen at fixed timing. When on-chip RAM is insufficient to hold the complete frame buffer, external PSRAM (Pseudo-Static RAM) is required. For example, 800x480 RGB565 requires 750 KB per frame, exceeding the on-chip RAM of most ESP32 models, so such RGB large-screen applications require PSRAM; lower-resolution RGB screens may not need PSRAM if the frame buffer fits in on-chip RAM.

QSPI And MIPI-DSI screen

Both types of screens can have either situation, depending on the driver IC model and working mode: some QSPI driver ICs have built-in GRAM, with usage similar to SPI, while others do not have GRAM and require the main controller to continuously output images. The ESP32-P4's MIPI-DSI (MIPI Display Serial Interface) uses Video Mode, with the frame buffer on the main controller side.

This is one of the most critical differences when choosing an interface:**SPI screens have low memory requirements for the main controller, while RGB screens depend on the frame buffer size; large, high-resolution screens typically require PSRAM**。

### 1.4 Refresh and screen tearing

Screen andWill notIndisplay immediately after writingCorrespondingContent,Driver IC willPressfixedofRefresh rate (such as 60 Hz）periodicGroundscan the video memory content to the panel. If the main controller writes GRAM ofThe process happens toOKSpanning one scan, both old and new frames will appear on the screen simultaneouslyofContent, calledIsScreen tearing (Tearing）。

Some screens with GRAM expose a TE (Tearing Effect) pin that outputs a sync pulse during the gap between two scans. The main controller can wait for this signal before starting to write, keeping the writing process synchronized with the screen scan. To completely eliminate tearing, the write direction must match the scan direction, and the write speed must keep up with the scan speed. Applications that mainly display static content and only occasionally update the screen may not need to handle tearing issues.

For More explanation about tearing, please refer to [ESP-IoT-Solution Programming Guide - LCD Screen Tearing Explained](https://docs.espressif.com/projects/esp-iot-solution/zh_CN/latest/display/lcd/lcd_screen_tearing.html)。

### 1.5 Full-screen refresh and window refresh

with GRAM ofThe screen supports window addressing (Window Addressing）:firstViaCommand specifies a rectangleArea，written afterofpixel data only fallsInthe/thisAreaInside. Therefore, when updating the screen, there is no need to retransmit the entire frame, just refreshChangeofpartial is enough, this can significantly reduceLowtransfer volume.LVGL etc.graphicsLibraryof“Local refresh" is implemented based on this mechanismof。

Information

e-ink screenof“Partial refresh”Is another concept, it involves electrophoretic particlesofDriving waveform, detailsSee [E-Paper chapter](../../ESP32-Peripheral-Tutorials/Display/E-Paper.md)。

### 1.6 Display orientation and coordinate system

The screen's native orientation is determined by the row and column arrangement of the panel and driver IC. With a native width of `sendBuffer()`, height is `sendBuffer()` ofscreenIsExample, usually defines the top-left corner asIscoordinate origin `sendBuffer()`, with the X-axis increasing to the right and the Y-axis increasing downward. After the module is installed in a device, the physical orientation of the screen may differ from the native orientation, so the display content needs to be rotated to the corresponding direction.

After the display direction changes, the logical coordinate system used by the graphics library also changes. Taking clockwise rotation of the native display as an example, the original coordinates `sendBuffer()` The relationship with rotated coordinates is as follows:

|Clockwise rotation angleLogical width × heightOriginal coordinates mapped to rotated coordinates
|0°`sendBuffer()``sendBuffer()`
|90°`sendBuffer()``sendBuffer()`
|180°`sendBuffer()``sendBuffer()`
|270°`sendBuffer()``sendBuffer()`

In actual development, you usually don't need to manually calculate these coordinates; the screen driver or graphics library provides display orientation settings. Based on where pixel conversion occurs, rotation can be divided into two methods:

|Rotation methodImplementation principleFeature
|Controller-side rotation (commonly called hardware rotation)Driver ICs with GRAM change the pixel addressing direction through row/column swapping and axis flipping; some driver ICs use registers such as MADCTL to controlUsing the driver IC's own addressing capability, the main controller doesn't need to rearrange the full frame of pixels, nor does it need an additional full rotation buffer; the available directions depend on the driver IC
|Controller sideSoftwarerotationCPU, software canvas, or GUI Framework first converts the frame buffer, then writes the converted pixels to the screenDoes not rely on the driver IC for full rotation capability, but increases memory usage, data copying, and computational overhead

for 0°、90°、180°、270° rotation, ifDriver IC to support the required direction, should prioritizeUseController-side rotation. It directlyUseScreen controllerofaddressingFunction，configurationCompleteAfter still canPressafter rotationofDraw with logical coordinates.Driver IC does not support row/column swap,Oror/personApplicationneed other anglesAndwhen the image is transformed, thenUseController sideSoftwarerotation.

Arduino_GFX's `sendBuffer()`、u8g2 constructorIn `sendBuffer()`～`sendBuffer()` etc.InterfaceUseat/inSettingsdisplay orientation.SettingsAfter, graphicsLibrarywillPresstoward the target directionAdjustLogical coordinatesAndwideHigh；pixel finalByDriver IC re-address,OrInController sideCompleteconversion, depends on the specific driverAndBuffering method.

Rotation parameters are subject to library and driver definitions

different graphicsLibraryFor rotation indexAndStarting directionofDefinitions are not exactly the same, someDriver IC also only supports axial flipping, cannotComplete 90° / 270° ofrow and column swap.SettingsBefore direction, should consultUsegraphicsLibrary anddriverofDescription；SettingsAfter, should read graphicsLibraryprovideofLogical widthAndHighdegrees, and check the four cornersofwhether the graphics are complete. When writing the driver yourself, you also need to confirm different directionsUnderwindow addressing offset.

touch IC usually stillPresstouch panelofNative orientationOutputcoordinates. After the display direction changes, the touch coordinates need to perform the sameofaxis swapAndMirror transformation, otherwise the touch position willAndScreen misalignment. SpecificHandleMethodSee 。

## 2. Display interface comparison

screenAndbetween main controllers oftenSeeofInterfaceAs shown in the table below. Main differencesInat/inData lineQuantity,GPIO occupyUse、transfer bandwidth and whether the screen side has GRAM。tableIn IO NumberIsapproximate range,Reset、backlightetc.optionalPinnot included; actual bandwidth depends onInterfaceClock,BusWidthAndspecificHardwareconfiguration:

|InterfaceData lineApproximate IO count usedBandwidth characteristicsScreen-side GRAMTypical applications
|I2C1 (bidirectional)2 (shared with other I2C devices)Low, usually ≤ 1 MbpsYes0.96 inchetc.small monochrome OLED
|SPI (4-wire)14～5Commonly 40 to 80 MbpsYesColor displays, e-paper displays, RLCD below 3.5 inches
|QSPI46～7High, theoretically up to 4 times that of SPIDepends on the driver IC1～2 inchHighResolution AMOLED
|I80 (8080 parallel port)8 / 1611～20HighYesSmall and medium-sized LCD
|RGB parallel port8 / 16 / 2412～28High**None**3.5 inch and larger screens
|MIPI-DSI1-2 data lanes, plus 1 clock lane4 to 6 (i.e., 2 to 3 differential pairs)Very high, typically Gbps levelNone (Video Mode)High-resolution screen (ESP32-P4)

### 2.1 SPI: The most commonly used interface

SPI is ESP32 Most commonlySeeofscreenInterface，PinFew, simple wiring, almost allYesseriesof ESP32 All supported. Typicalof 4 line/wire SPI screenModulePinAs follows (AndWavesharescreenModuleofSilkscreenCorresponding）:

|PinDescription
|VCC / GNDPower
|DIN(MOSI)Data line，main controller → screen
|CLK(SCLK)Clock line
|CSChip select, active low
|DCCommand/Data selection: LOW indicates sending commands, HIGH indicates sending pixel data
|SDO(MISO)Optional data line, screen -> MCU, used to read ID, status, or display memory. Most applications only write and do not read, so it can be left unconnected
|RSTReset, active low. Some modules can omit this; fill the reset pin parameter in the driver code with `sendBuffer()`
|BLBacklight control (LCD-specific, can connect PWM to adjust brightness)

Among them DC PinIs display SPI compared to ordinary SPI ExtraofkeySignal:screenDriver IC Viathe/thisPinofLogic level, distinguishing receivedofbytes are commandsOrdata.RST Can be omitted, toModuleschematicAndDriver requirementsPrevails.

### 2.2 frame rateofthree stages

The commonly referred to "frame rate" is actually limited by three stages; the actual smoothness of the display is limited by the stage with the lowest throughput:

-

**Render frame rate**: The rate at which the main controller generates image data. It depends on the controller's performance and the image complexity (partial updates are typically faster than full-screen updates), and is dynamically changing.

-

**Interface frame rate**: The rate the display interface can achieve when the MCU continuously transmits a complete frame. Determined by the interface bandwidth and the amount of data per frame:

```
Interface frame rate limit ≈ interface bandwidth ÷ frame data size
```

This formula gives the theoretical upper limit; actual values will be lower, so margin should be reserved when estimating.

-

**Screen refresh rate**: The rate at which the driver IC scans display memory content to the panel (see [Refresh and screen tearing](#refresh-and-tearing)）。SPI / I80 screenByDriver IC decide on your own (canViaCommand configuration),Andmain controlleroftransmission rateNoneOff;RGB Screen does notYesthis layer of decoupling, the main controllerOutputofThe process itself is screen scanningofprocess.

Why the actual frame rate is lower than the estimated value

SPI、QSPI And I80 store/saveInCommand, transaction intervalAnd DMA BlockofOverhead;RGB And MIPI-DSI Video Mode Also need to countYesoutside the effective screenofblanking timing.In addition,MemoryAnd DMA bandwidth may also becomeIsbottleneck.

Interface frame rateIs the most needed estimate during selectionofStep. Taking 40 MHz of SPI driver 240×320 RGB565 screenIsExample:one frame is approximately 1.23 Mbit，Interface frame rateUpper limit approximatelyIs 40 Mbps ÷ 1.23 Mbit ≈ 33 frame/Seconds (actuallyYescommandAndprotocol overhead, will be moreLowsome). This estimate can explain several commonSeePhenomenon:

- SPI driver 240×240、240×320 thisClassMedium-lowResolutionWhen using the screen, it usually meets basic GUI ofRequirement；ResolutionContinue to mentionHighAfter, full screen refreshofbandwidth is veryFastbecome/intoIsbottleneck.
- ESP32 ecosystemInsmall sizeHighResolution AMOLED Module (such as 410×502、466×466）Multi-samplingUse QSPI，to alleviate single-line SPI ofbandwidth pressure.
- 800×480 thisClassone frame of a large screen 750 KB，UseSingle wire SPI Full-screen transmission is difficult to achieve smooth GUI requiredofframe rate, therefore thisClassScreens mostly useUse RGB、QSPI Or MIPI-DSI etc.bandwidth moreHighofInterface。

If the estimated interface frame rate is sufficient but the actual display is still not smooth, the bottleneck is typically in the rendering frame rate; you should optimize the drawing logic or reduce the image complexity, rather than increasing the interface clock.

### 2.3 Summary of interfaces

- **I2C**: Fewest pins (shares bus with other I2C devices), but lowest bandwidth, only suitable for small monochrome OLEDs.
- **SPI**: Only uses 4-5 GPIOs, supported by all ESP32 series, making it the most universal choice; bandwidth is limited, so at higher resolutions, other interfaces need to be used.
- **QSPI**:will SPI of 1 rootData lineExtensionIs 4 pins, pixel data stageEachClock transmits at most 4 bit，Theoretical peakIs SPI of 4 times, isHighResolution AMOLED ofmainstreamInterface。
- **I80**: Use 8 bitOr 16 bit parallel dataBus，bandwidthHighfor single-line SPI，But adding WR、DC、CS etc.controlSignalAfter usually needs 11～20 a/each GPIO, inPinTightof ESP32 on/upUseLess.
- **RGB**: When the main controller directly outputs panel timing, the bandwidth is high and cost is low, but it uses many GPIOs and must maintain a full frame buffer on the controller side (large screens typically rely on PSRAM).
- **MIPI-DSI**: A high-speed differential serial interface commonly used for high-resolution, high-refresh-rate screens, with few pins and high bandwidth; currently only supported by the ESP32-P4.

## 3. ESP32 display interface support

Different ESP32 series support different display interfaces; you need to verify the MCU capabilities before selecting a screen:

|InterfaceSupported ESP32 series
|I2C / SPI / QSPIFull series (via generic I2C / SPI peripherals)
|I80 (Native)ESP32、ESP32-S2（I2S of LCD Mode）、ESP32-S3、ESP32-S31、ESP32-P4（dedicated/specializedUse LCD Peripheral）
|I80 (Simulated)For Models without a native I80 peripheral, such as ESP32-C5 / C6 / H2, the Parlio peripheral can be used to emulate I80 timing
|RGBESP32-S3、ESP32-S31、ESP32-P4
|MIPI-DSIESP32-P4

Each seriesofSpecific support status is based on [ESP-IoT-Solution: LCD Development Guide - Supported Interface Types](https://docs.espressif.com/projects/esp-iot-solution/zh_CN/latest/display/lcd/lcd_development_guide.html#id2) Prevails.

Based on display interface and application scale, you can refer to the following selection methods:

- ESP32-P4 Interfacethe most complete, isHighResolutionScreen'sPreferred, but itself does not haveNoneline connection,Development Boardusually additionally configured with ESP32-C6 etc.Moduleprovide Wi-Fi And Bluetooth；
- With its dedicated LCD peripheral and optional large-capacity PSRAM, the ESP32-S3 is currently the most commonly used Model for display applications;
- The newly released ESP32-S31 supports the same interface types as the ESP32-S3, with higher measured frame rates for the same specification RGB screens;
- ESP32-C3 / C6 etc. primarily use general-purpose SPI, suitable for small to medium-sized SPI screens.

Measured frame rates of various models at different interfaces and resolutions can be referenced from [Espressif LCD Application Development Notes](https://docs.espressif.com/projects/esp-techpedia/zh_CN/latest/esp-friends/advanced-development/lcd-application-note/overview.html) LVGL Benchmark data table in.

In the Arduino ecosystem, the differences among common SPI, QSPI, I80, and RGB screens are mostly encapsulated by graphics driver libraries. Commonly used libraries include:

|LibraryApplicable scope
|GFX Library for Arduino (Arduino_GFX)Universal color screen graphics library, supporting SPI / QSPI / I80 / RGB multiple buses and numerous driver ICs
|LovyanGFXcolor screen graphicsLibrary，excellent performance, supports touchAndmultiple kindsBus
|TFT_eSPIClassic SPI color screen graphics library, adapts to screens via configuration files
|u8g2Monochrome screen graphics library, commonly used for OLED / RLCD, with rich font Resources
|GxEPD2e-ink screenDriver library
|ESP32-HUB75-MatrixPanel-DMAHUB75 LED Matrix driver library, using DMA buffer
|WaveshareExample program packageOfficial drivers and examples for various Development Boards / screen modules, with pins and parameters already adapted

eachClassScreens are respectively suitable forUseWhichLibrary, inCorrespondingchapter/sectionAnd [GUI Framework](../../ESP32-Peripheral-Tutorials/Display/GUI.md) will be introduced in detail in the chapter. Newer interfaces such as MIPI-DSI currently mainly rely on ESP-IDF and related display components.

Tip

UseWaveshareofwith screenDevelopment Board（as/like ESP32-S3-Touch-LCD series), the screen is already directly connected to the main controller,Nonerequires wiring; officialExample program packageInPin definitionAndinitializationParameterCan directlyUse。

## 4. with GRAM Screen'sGeneraldriver flow

For those withDriver IC And GRAM of SPI、I2C、I80 Screen, the driving process usually includes the following steps, understanding this processYeshelps with reading subsequent chaptersofExampleCode:

- **Initialize bus**: Configure pins and clock frequencies for interfaces such as SPI / I2C.
- **Reset screen**: Pull the RST pin low for a period and then release it, returning the driver IC to a known state.
- **Send initialization sequence**:to/towardDriver IC write a series of configuration commands,SettingsPower、Scan direction, pixel formatetc.。this string of commandsByDisplay manufacturers calibrate for specific panels,GeneraldirectlyUseWaveshareExampleOrDriver libraryInready-madeofinitializationCode，Noneneed to write yourself.
- **Settingswrite window**: Specify the rectangular area to be updated next (full screen or partial).
- **Write pixel data**: Continuously write pixels to the area using the selected pixel format (e.g., RGB565), and the image will be displayed accordingly.

Subsequent screen updates simply repeat steps 4 and 5. When using libraries like Arduino_GFX and u8g2, steps 1-3 are handled by the library's `sendBuffer()` are completed, and steps 4 and 5 are encapsulated as `sendBuffer()`、`sendBuffer()`、`sendBuffer()` and other drawing APIs. Whether the drawing API immediately transfers pixels to the screen or first writes to the MCU-side buffer, then `sendBuffer()` Or `sendBuffer()` unified transmission, depending on theUsegraphicsLibraryand itsBuffer mode。

Other types of screens differ in this process: RGB and MIPI-DSI Video Mode screens do not use the set window then write to screen-side GRAM approach; applications draw pixels in the main controller-side frame buffer, and the LCD peripheral continuously outputs the frame buffer content to the screen according to pixel clock and sync timing; after writing to the display RAM, e-ink screens also need to send a refresh command and wait for the BUSY signal to release.

## 5. Extended reading

- [Espressif ESP-IoT-Solution: LCD Development Guide](https://docs.espressif.com/projects/esp-iot-solution/zh_CN/latest/display/lcd/lcd_guide.html):officialIndocumentation, based on esp_lcd / ESP-IDF，includes eachInterfaceofParameterComparison, typicalConnection methodAnd LCD Glossary, can be used asIsthis chapterofAdvanced supplement.
- [Espressif ESP-Techpedia: LCD Application Development Notes](https://docs.espressif.com/projects/esp-techpedia/zh_CN/latest/esp-friends/advanced-development/lcd-application-note/index.html)

---

After understanding these common concepts, you can proceed to the chapters on specific screen types. If you have not yet determined the screen type, you can return to [Displays overview](../../ESP32-Peripheral-Tutorials/Display/index.md) See Selection Comparison.

