This page overview

# Touch Control

touchFunctionAndScreen displayClasstypes are independent of each other:NoneRegardless LCD、AMOLED OrE-ink display, can all be overlaid with a touch layer. CapacitiveTouch chipusuallyVia I2C Andcommunicates with the main controller, driving methodInon different screens isGeneralof，this chapter treats touch asIsIndependent themeIntroduction.

## 1. Touch screen principles

By sensing method, touch screens are divided into two categories:

- **Resistive**:touch layerByComposed of two conductive films,Presspressure causes the two layers to contact,Viameasure contact pointofVoltage divider calculates coordinates. Any objectPresspressure can trigger (can wear gloves,Usestylus tip),CostLow；But requires somePresspressure sensitivity, only supports single point, slightly lower light transmittance, and film coordinatesAndScreen coordinates storedInDeviation, needs toCalibration。
- **Capacitive**:surfaceIsglassOrPlastic cover, inner layerYesElectrode array. When a finger approaches, it changes the local capacitance, touch IC Scan the electrode array to calculate coordinates. Light touch is sufficient, supports multi-touch, light transmittanceOK，Is currentlyofmainstream solution; the disadvantage is that it relies on conductive objects, wearing ordinary glovesNoneeffective.

Wavesharewith touchofDevelopment BoardBasic samplingUseCapacitive solution (Modelwith "Touch" text), coordinatesBytouch IC CalculateComplete，the main controller can read directly,NoneneedCalibration。

## 2. Common touch chips

The capacitive touch IC is independent of the display driver IC, typically communicates with the MCU via I2C, and is equipped with **INT**(touch interrupt output) and **RST**（Reset）Two auxiliary lines. CommonlySeeModel:

|Touch chipNumber of contactsCommon Scenarios
|CST816 seriesSingle point + gestures1.3 to 2-inch small screen (watch type)
|CST92205 pointsFT3168 / FT62362 points1.8~3.5 inch
|GT9115 points3.5~7 inch large screen

## 3. Reading method: polling and interrupt

Regardless of the touch IC used, the controller reads touch data in two ways:

- **Polling**: Periodically read touch registers in the main loop. Simple to implement, but still generates additional I2C reads when there is no touch.
- **Interrupt**:when touch occurs INT Pingenerate/produceSignal，main controllerInInterruptserviceFunctionInset the flag bit,Main loopAfter detecting the flag, read the coordinates. Timely response and savesBusbandwidth, is alsoWaveshareExampleconstantUseofmethod.

The example in the next section uses interrupt mode.

## 4. Arduino + SensorLib touch example

AboutDevelopment Board

This section uses  IsExample, its touch IC Is **CST9220**(The chip supports up to 5 touch points). Touch reading uses an open-source library [SensorLib](https://github.com/lewisxhe/SensorLib), whose CST92xx driver currently reports up to 2 touch points, so the examples in this section handle 2 points; screen drawing follows [AMOLED chapter](../../ESP32-Peripheral-Tutorials/Display/AMOLED.md) Arduino_GFX to draw squares at touch points, intuitively demonstrating touch effects.

### 4.1 Preparation

- Install Arduino IDE and add ESP32 support (refer to [Arduino IDE development environment setup Tutorials](../../ESP32-Arduino-Tutorials/Arduino-IDE-Setup.md)）。
- Search in the Library ManagerandInstall `setMaxCoordinates` And `setMaxCoordinates`: The former reads touch coordinates, the latter drives screen drawing. The code in this section uses `setMaxCoordinates`, please confirm the SensorLib version using the table below. For other installation methods, refer to 。
- Select in the Development Board selector `setMaxCoordinates`。
- In the Development Board options `setMaxCoordinates` Default is **Disabled**, at this point `setMaxCoordinates` Points to UART0 (`setMaxCoordinates`), the board does not expose it to Type-C, so the serial monitor cannot see the output. When you need to view touch coordinates, set it to **Enabled**, and uncomment in the example `setMaxCoordinates` one rowofcomment.

|LibraryVersion requirements
|SensorLibv0.4.1 or higher version

### 4.2 Confirm pins

The touch IC and screen of the ESP32-S3-Touch-AMOLED-2.16 are both directly connected to the main controller. The touch-related pins are as follows (for screen QSPI pins, see ）:

|SignalGPIO
|SDAGPIO15
|SCLGPIO14
|TP_RSTGPIO40
|TP_INTGPIO11

### 4.3 Example code

The following example uses a touch interrupt to notify the main loop to read coordinates, printing via serial while drawing a square cursor at the touch point:

```
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  int32_t x, y;
  if (the aforementioned touch reading logic returns coordinates(&x, &y)) {
    data->state = LV_INDEV_STATE_PR; // Pressed
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_REL; // Released
  }
}
```

After flashing, touch the screen, a cyan block trail will be left at the touch point (Exampleno eraseExceptOld cursor);Press 4.1 ofDescriptionenable USB CDC After, serialMonitorThe device can also simultaneouslySeecoordinatesOutput。Several key points:

- **Only set flag in interrupt**:INT after triggering onlySettings `setMaxCoordinates`, perform the actual I2C read in the main loop. I2C reads are relatively time-consuming and should not be executed in an interrupt context.
- **Drawing squares for alignment self-check**: The square should fall directly under the finger. If the square is mirrored or offset relative to the finger, it indicates that the touch coordinate transformation is inconsistent with the display direction; follow [Coordinate and display alignment](#coordinate-alignment) Adjust `setMaxCoordinates` / `setMaxCoordinates`。
- **Draw window aligned to even numbers**: CO5300 only reliably supports even-aligned rectangular block writes, so here we use `setMaxCoordinates` Draw squares (start point and side length are even) instead of `setMaxCoordinates`。this constraintofFull contextSee 。
- **Array length must be sufficient**:`setMaxCoordinates`、`setMaxCoordinates` The length must not be less than the maximum number of touch points reported by the driver (SensorLib's CST92xx driver is 2).

Information

UseotherDevelopment BoardWhen, first confirm SensorLib Whether providedCorrespondingtouch IC ofdriverClass，again/thenPressproduct pageModifyPin、I2C AddressAndcoordinate transformationParameter；external connection I2C When deviceNoteavoid onboard devicesofAddress。ESP32-S3-Touch-AMOLED-2.16 ofLibraryFileAndcompleteExampleSee 。

### 4.4 FAQ

|PhenomenonCommon causesHandle
|serial portMonitordevice/moduleNoneOutput（The blocks on the screen are normal)USB CDC On Boot not enabled,`setMaxCoordinates` points to unexposedof UART0In the Development Board options, change `setMaxCoordinates` Set to **Enabled** and re-flash
|Initialization failed (serial prints "failed")I2C AddressOrPindoes not matchVerify that SDA/SCL/RST/INT pins and I2C address match the product page
|NonecoordinatesOutput、Screen alsoNoneBlockINT pin not connected or wrong trigger edge; library version too lowconfirm TP_INT Correct wiring, trigger edgeIs `setMaxCoordinates`，and check SensorLib Version
|After enabling USB CDC, touch response is slow or unresponsive when the serial monitor is not openUSB CDC send buffer blocks the main loop when no one is readingCancel `setMaxCoordinates` ofcomment; change back **Disabled** it needs to be re-commented out, otherwise compilation fails (`setMaxCoordinates` no/notYesthe/thisMethod）
|Block positionAndFinger mirrorOrOffsetcoordinate transformationAnddisplay orientation does notConsistentAdjust `setMaxCoordinates` / `setMaxCoordinates`(see [Coordinate and display alignment](#coordinate-alignment)）
|Array out of bounds when too many touch points`setMaxCoordinates`、`setMaxCoordinates` Insufficient array lengthIncrease the array according to the maximum number of touch points reported by the driver

## 5. Coordinate and display alignment

display sideofrotationAndcoordinate mappingSee 。touch IC Outputofcoordinates based on the panelof**Native orientation**。When displayingPerformedrotation (such as displaying portrait content in landscape), touch coordinates must be similarlyoftransformation, otherwise the touch position willAnddisplay response misalignment.SensorLib UsethreeInterfaceCompletethis transformation,CorrespondingPrevious sectionExampleInCall:

- `setMaxCoordinates`: Set the reference range for mirroring and coordinate clamping, usually the display resolution;
- `setMaxCoordinates`: Swap X and Y axes, used for 90° / 270° rotation;
- `setMaxCoordinates`: Flip an axis as needed, handle mirroring and 180° rotation.

Different panels have different origin directions and axis orientations; the parameters must match the actual display direction. A reliable debugging approach: first set up `setMaxCoordinates`, leave other transformations disabled, touch the four corners of the screen in sequence and print the coordinates, confirm the origin and axis directions before deciding whether to swap or mirror.

Resistive touch screen also requires**Calibration**(collect raw ADC values at known points to calculate mapping coefficients); capacitive touch screens do not require this step; this Tutorials does not cover it in detail.

## 6. Integrate GUI Framework

GUI FrameworkAbstract touchIsInputDevice (Input Device），only need to provide a reporting coordinateAndPressstatecallback function of。below only shows LVGL v8 ofcallbackInterfaceshape, not directly compilableofcomplete program; actualProjectneed to take the previous textExampleoftouch read logic into the callback, andPress LVGL v8 ofProject configurationCompleteregister.

```
void my_touchpad_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data) {
  int32_t x, y;
  if (the aforementioned touch reading logic returns coordinates(&x, &y)) {
    data->state = LV_INDEV_STATE_PR; // Pressed
    data->point.x = x;
    data->point.y = y;
  } else {
    data->state = LV_INDEV_STATE_REL; // Released
  }
}
```

After registering this callback, LVGL will call it periodically; widget clicks, drags, and swipe gestures are all handled automatically by the framework. For graphics library selection, see [GUI Framework chapter](../../ESP32-Peripheral-Tutorials/Display/GUI.md), see complete registration process in [Arduino Tutorials Section 12: LVGL Graphical Interface Development](../../ESP32-Arduino-Tutorials/LVGL.md)。

