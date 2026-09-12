This page overview

# GUI Framework

Lighting up the screen is only the first step; drawing the interface also requires a graphicsLibrary。graphicsLibraryAndspecificofDriver IC Basic decoupling:as long as it is provided with "write a block of pixel data to the screen"ofdriverFunctionWill do; whileScreen typeDetermines the availableofLibrary，monochrome screen, color screenAndEach e-ink displayYesCorrespondingofSolution. This chapter introduces ESP32 ecosystemInconstantUseofgraphicsLibraryAnd its selection approach.

## 1. Layering of graphics libraries

The structure of each graphics library is basically the same, divided into three layers from top to bottom:

- **Widgets / Application layer**: ready-made widgets like buttons, labels, charts, as well as layout, animation, and event systems. Only a complete GUI Framework (such as LVGL) provides this layer.
- **Drawing engine**: Drawing of points, lines, rectangles, text, and bitmaps; managing the drawing buffer in memory. The core of all graphics libraries.
- **Driver interface (HAL)**: The graphics library does not directly operate hardware, but interacts with hardware through screen refresh functions and input functions.

Previous chapters have already demonstrated this layeringofTwo forms:

- Arduino_GFX、u8g2 thisClass**Graphics library**Includes drivers for common driver ICs; select the corresponding constructor to use, and the drawing API is ready to use;
- LVGL type**GUI Framework**does not containHardwareDriver, need to "refresh screenFunction”（usuallyBy Arduino_GFX etc.Graphics libraryOr esp_lcd Provided)And“touch readFunction”(see [Touch Control chapter](../../ESP32-Peripheral-Tutorials/Display/Touch.md)) register to it, see architecture details in [Arduino Tutorials Section 12](../../ESP32-Arduino-Tutorials/LVGL.md)。

## 2. Selection method and graphics library comparison

First determine candidate libraries based on the screen type and project requirements using the following paths, then use the comparison table to view the functional differences between candidate libraries.

### 2.1 How to choose

canPressScreen typeAndRequirementfollow the path belowSelect:

- **Monochrome screen (OLED / RLCD) → u8g2**. The 1bpp buffer matches the screen's native format, with rich fonts and minimal memory usage. See example in [OLED chapter](../../ESP32-Peripheral-Tutorials/Display/OLED.md) And [RLCD chapter](../../ESP32-Peripheral-Tutorials/Display/RLCD.md)。
- **E-Paper → Waveshare official driver library or GxEPD2**. E-ink screen full/partial refresh control requires dedicated library support, see [E-Paper chapter](../../ESP32-Peripheral-Tutorials/Display/E-Paper.md). In principle, LVGL could also be ported to e-Paper displays, but second-level refresh rates do not match the interaction assumptions of a GUI Framework, so this organization is not recommended.
- **Color screen, only for data display (gauge readings, status pages, no complex interaction) -> Draw directly with Arduino_GFX**. No framework needed, small code size, see [LCD chapter](../../ESP32-Peripheral-Tutorials/Display/LCD.md) example.
- **Color screen, requires interactive UI (buttons, lists, animations, touch) → LVGL**。WidgetAndThe event system can reduce hand-writtenCode，in conjunction with [SquareLine Studio](https://squareline.io/) and other tools can also visually design interfaces. Note that LVGL requires an additional draw buffer, which doesn't need to be the full screen, but for high-resolution screens, you should still verify PSRAM availability (see [AMOLED chapter](../../ESP32-Peripheral-Tutorials/Display/AMOLED.md) memory estimation).
- **LED Matrix → ESP32-HUB75-MatrixPanel-DMA**。the/thisLibraryOriented towards HUB75 pointsmatrix screen,See [LED Matrix chapter](../../ESP32-Peripheral-Tutorials/Display/LED-Matrix.md)。

If stillNoneCannot be determined, can firstFrom Arduino_GFX（color screen)Or u8g2（monochrome screen):Both are easy to get started with; introduce them later when complex interfaces are needed LVGL，alreadyYesofdriverCodeCan still be reusedUse。

### 2.2 constantUsegraphicsLibrarycomparison table

|LibraryLocateTarget screenWidget systemInput / touchMemory overhead
|[LVGL](https://lvgl.io/)Complete GUI FrameworkPrimarily color screens, monochrome configurableRich (buttons/lists/charts/animations)Provides input event framework, touch IC reading needs custom adaptationMedium to high (drawing buffer + framework overhead)
|[u8g2](https://github.com/olikraus/u8g2)Monochrome drawing libraryOLED / RLCD / monochrome LCDNoneNoneLow (1bpp buffer)
|[Arduino_GFX](https://github.com/moononournation/Arduino_GFX)Color screen drawing librarySPI/QSPI/I80/RGB color screenNoneNoneLow (can write directly without frame buffer)
|[LovyanGFX](https://github.com/lovyan03/LovyanGFX)Color screen drawing librarySPI/I80/RGB color screenNoneIntegrates common touch ICsLow~Medium
|[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)Color screen drawing librarySPI color screen, some platforms support parallel interfaceNoneBuilt-in XPT2046 supportLow~Medium
|[GxEPD2](https://github.com/ZinggJM/GxEPD2)e-ink screenDriver librarySPI E-PaperNoneNoneLow~Medium（Can buffer only a few lines, multi-color panels even more soHigh）
|[ESP32-HUB75-MatrixPanel-DMA](https://github.com/mrcodetastic/ESP32-HUB75-MatrixPanel-DMA)HUB75 pointsmatrix screenDriver libraryLED MatrixNoneNonein (DMA buffer)

Additional notes:

- **APIs between graphics libraries are highly similar**: Arduino_GFX, LovyanGFX, and TFT_eSPI have similar API styles (`setCursor`、`setCursor`、`setCursor` etc.），masterAmong themone type is sufficientClasspush othersLibraryofUseMethod.
- **This Tutorials uses Arduino_GFX for all color screen examples**: It covers a wide range of buses and driver ICs, and the example packages for most Waveshare display Development Boards are also based on it.
- **LVGL cannot work independently**: It is responsible for widget and interface rendering; screen refreshing is handled by registered callbacks, which typically call the graphics library or esp_lcd.
- LVGL v8 And v9 of API Yesincompatible changes,Useso/theUseDevelopment BoardExamplepackage providesofVersionWill do, do not mixUse；SquareLine Studio etc.VisualizationToolExportofCodealso need toAndProjectUseofmainVersionMatch.

## 3. Further reading

- [Arduino Tutorials Section 12: LVGL GUI Development](../../ESP32-Arduino-Tutorials/LVGL.md):LVGL ofArchitecture, portingAndcompleteDevelopment process。
- [OLED chapter](../../ESP32-Peripheral-Tutorials/Display/OLED.md):u8g2 ofConstructor,Buffer modeAndPractice.
- eachWavesharewith screenDevelopment Boardofproduct page:CorrespondingScreen'sofficialExamplepackage/bagInusually also providesGraphics libraryAnd LVGL twoClassExample program, is the most fittingHardwareofstarting point.

