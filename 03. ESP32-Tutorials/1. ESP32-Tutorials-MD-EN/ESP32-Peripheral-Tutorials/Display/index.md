This page overview

# Displays

Screens are the primary interaction method for embedded devices. Common screens in the ESP32 ecosystem, classified by display principle, mainly include LCD, OLED, and E-Paper, all of which have pixel structures integrated on the entire panel: LCD has a reflective variant RLCD, and OLED is divided into PMOLED and AMOLED. Additionally, there is LED Matrix, where each pixel is an independently packaged LED bead, forming its own category. Each type has different trade-offs in color, power consumption, refresh speed, and readability; selection should be based on specific application scenarios.

This chapter first compares the differences between various screen types for selection reference; the display principles and driving methods for each type are introduced in separate sections.

## 1. Screen Types Overview

[SVG diagram]

- **[LCD](../../ESP32-Peripheral-Tutorials/Display/LCD.md)**: Relies on backlight passing through the liquid crystal layer to form images; it is a versatile, cost-effective color screen solution widely used in various devices with GUIs.
- **[RLCD](../../ESP32-Peripheral-Tutorials/Display/RLCD.md)**: reflective **LCD**, utilizing ambient light for imaging without a backlight, with very low power consumption and clear readability under sunlight, suitable for Always-On low-power devices.
- **[OLED](../../ESP32-Peripheral-Tutorials/Display/OLED.md)**:pixel self-emissive,Nonerequires backlight, contrastHigh。constantSeeof 0.96 inchetc.small monochromeModuleBelongs to PMOLED（passive matrix OLED），suitable for displaying statusInformation。
- **[AMOLED](../../ESP32-Peripheral-Tutorials/Display/AMOLED.md)**: Active-matrix OLED (as opposed to PMOLED), full-color, high contrast, high resolution, commonly found in small devices such as smartwatches.
- **[E-Paper](../../ESP32-Peripheral-Tutorials/Display/E-Paper.md)**: Electrophoretic ink imaging; zero power consumption when maintaining a static image, with a paper-like appearance, but slow refresh, suitable for low-frequency update scenarios such as electronic price tags and calendars.
- **[LED Matrix](../../ESP32-Peripheral-Tutorials/Display/LED-Matrix.md)**:ByIndependent packageof LED LED beadPressrow and column arrangement,Eacha pixel is an LED bead,BrightnessHigh、Can be spliced into a large screen, but with large dot pitch,ResolutionLow，Suitable for advertising screens,Informationscreenetc.viewing from a distanceofScenario。

## 2. Selection Comparison

When selecting, first refer to the following questions to determine the screen type of interest, then use the table below to compare specific differences:

- Need a color GUI? → LCD or AMOLED; only displaying text and icons? → OLED, RLCD, or E-Paper.
- Battery-powered and need long battery life? -> Choose E-Paper for mostly static displays, choose RLCD for always-on displays with real-time updates.
- Use under bright light? → Prefer reflective displays like RLCD or E-Paper; need large screen, long-distance viewing with high brightness? → LED Matrix.
- Pursuing display quality? → AMOLED has the best contrast and color performance, but also the highest cost.

Note for Outdoor Use

“Readability under sunlight”Only represents the screenInWhether it is easy to see clearly under strong ambient light does not mean the screen can be exposed for a long timeInunder sunlightUse。
long-term direct sunlight may bringHightemperature, UV aging, adhesive layer aging, polarizer deterioration, uneven display, contrast degradationetc.Problem.Even if RLCD And E-Paper thisClassReflectiveScreen, should alsoAccording toactualApplicationSelectWith outdoor protection, temperature range, waterproof and moisture-proofAnd UV Protection designofModuleOrcomplete machine solution.

|Screen typeDisplay principleColorPower consumptionRefresh speedReadability under sunlightTypical applications
|LCDBacklight + LCD filterFull color(mainly backlight)FastGeneralGeneral GUI、dashboard
|RLCDReflects ambient light + LCDMonochrome / low colorExtremely lowFastExcellentAlways-on watches, outdoor devices
|OLEDOrganic pixel self-emissiveMostly monochrome / dual-colorLow (black consumes no power)FastGeneralStatus display, small gauges
|AMOLEDActive matrix self-emissiveFull color、HighcompareMedium-lowFastBettersmartwatches,HighTerminal interactive device
|E-PaperElectrophoretic inkBlackWhite / Three-color / colorStatic zero power consumptionslow (seconds level)ExcellentElectronic price tags, calendars, readers
|LED MatrixLED bead self-emissiveFull color / monochromeHigh (requires high current supply)FastExcellent (high brightness)Advertising screens, information displays, scoreboards

## 3. Chapter Contents

The technologies involving interfaces, touch, and GUI are independent of the specific screen type. The following sections introduce them separately:

- [Display Basics and Interfaces](../../ESP32-Peripheral-Tutorials/Display/Display-Basics.md): Common concepts such as pixels, color depth, and frame buffers, as well as interface differences among SPI/QSPI/I80/RGB/MIPI-DSI.
- [LCD](../../ESP32-Peripheral-Tutorials/Display/LCD.md)
- [RLCD](../../ESP32-Peripheral-Tutorials/Display/RLCD.md)
- [OLED](../../ESP32-Peripheral-Tutorials/Display/OLED.md)
- [AMOLED](../../ESP32-Peripheral-Tutorials/Display/AMOLED.md)
- [E-Paper](../../ESP32-Peripheral-Tutorials/Display/E-Paper.md)
- [LED Matrix](../../ESP32-Peripheral-Tutorials/Display/LED-Matrix.md)
- [Touch Control](../../ESP32-Peripheral-Tutorials/Display/Touch.md): Resistive and capacitive touch screens, common touch ICs, coordinate reading and rotation mapping.
- [GUI Framework](../../ESP32-Peripheral-Tutorials/Display/GUI.md):LVGL（Light and Versatile Graphics Library，Lightweight graphical interfaceLibrary）、u8g2 etc.frameworkofLocateAndselection.

