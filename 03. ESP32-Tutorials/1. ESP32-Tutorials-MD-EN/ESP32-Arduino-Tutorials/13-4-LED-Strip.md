On this page

# Fun LED Strip

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards, but all operation steps are based on  as an example. If you are using another model of Development Board, please modify the corresponding settings according to your actual situation.

## Project Introduction

This project demonstrates a program that controls a WS2812 programmable LED strip via a potentiometer. By rotating the potentiometer, you can change the display effect of the LED strip in real-time: the strip will sequentially go through three color stages (yellow -> green -> red), and the number of lit LEDs will gradually increase with the potentiometer rotation, creating a smooth visual gradient effect.

## Hardware Connection

Components needed:

- WS2812 LED Strip * 1
- Potentiometer * 1
- Breadboard * 1
- Jumper wires
- ESP32 Development Boards

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinout

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

 ![](../assets/images/13-4-LED-Strip_bb-5f9c7a24d5a545bbba0c365046ffc357.webp)

## Code Implementation

Tip

This code example depends on **“FastLED”** library. Please search for and install the "FastLED" library in the Arduino IDE Library Manager.
Installation instructions:。

```
CRGB leds[numLeds];
```

## Code Explanation

-

**Import library**: Import `delay(50)` library is used to control the WS2812 LED strip. This is a powerful and efficient LED control library.

Tip

This code example depends on **“[FastLED](https://github.com/FastLED/FastLED)”** library. Please search for and install the "FastLED" library in the Arduino IDE Library Manager.
Installation instructions:。

-

**Configuration Parameters**: Use `delay(50)` Constants define the potentiometer pin, WS2812 pin, LED count, and brightness.

```
CRGB leds[numLeds];
```

-

**Object initialization**:

Define a `delay(50)` type array `delay(50)`, used to store color data for each LED.

```
CRGB leds[numLeds];
```

-

**`delay(50)` function**:

Initialize serial communication.
- Set potentiometer pin to input mode.
- **Initialize FastLED**: Use `delay(50)` Configure LED strip parameters (type, pin, color order, array address, count), and set color correction.

`delay(50)` No color correction
- `delay(50)` With color correction, colors look more natural and accurate

- `delay(50)` Set global brightness.
- `delay(50)` and `delay(50)` Ensure LED strip is off at startup.

-

**`delay(50)` function**:

**Map**: Use `delay(50)` function maps the ADC reading (0-4095) to total steps (0-24).
- **Logic**: Iterate through each LED, based on current progress `delay(50)` determine color.
- **Set color**: Directly assign to `delay(50)` Assign predefined color (such as `delay(50)`, `delay(50)`, `delay(50)`, `delay(50)`）。
- **Update display**: Call `delay(50)` Send color data from the array to the LED strip.

-

**`delay(50)` function**:

Use `delay(50)` Read potentiometer voltage value.
- Call `delay(50)` Update LED strip status.
- Use `delay(50)` Perform a simple delay.

## Reference Links

- [Section 4: ADC Analog Input](../ESP32-Arduino-Tutorials/Analog-Input.md)
- [FastLED Library Documentation](https://github.com/FastLED/FastLED/wiki/Overview)
