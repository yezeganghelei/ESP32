On this page

# Pulse Width Modulation (PWM)

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards, but all operation steps are based on  as an example. If you are using another model of Development Board, please modify the corresponding settings according to your actual situation.

## 1. What is PWM?

**PWM (Pulse Width Modulation)** is a kind ofDigital signalModulation technology, canUseTo "simulate"Continuous changeofvoltage.

when you need to control the deviceOutputof“intensity" (For exampleadjust LED BrightnessOrmotor speed), not just simpleofopen/on/in the off state,**PWM (Pulse Width Modulation)** technology is an effective solution.

PWM signals are essentially still digital signals with only HIGH and LOW states, but by controlling the time proportion of the high level, effects similar to analog output can be achieved.

![](../assets/images/05-PWM2-adee0a8ecd18f9eeca2fbe8ad126f150.webp)

Core concepts of PWM include:

-

**Duty Cycle:** The percentage of time the high level (signal is HIGH) lasts within one PWM cycle. The duty cycle directly affects the average power or intensity of the output.

Different duty cycles produce different average voltage effects:

**Duty cycle 0%**: Average voltage ≈ 0V
- **Duty cycle 25%**: Average voltage ≈ 0.825V (3.3V * 0.25)
- **Duty cycle 50%**: Average voltage ≈ 1.65V (3.3V * 0.50)
- **Duty cycle 75%**: Average voltage ≈ 2.475V (3.3V * 0.75)
- **Duty cycle 100%**: Average voltage ≈ 3.3V

-

**Frequency:** The number of times a PWM signal repeats a complete cycle per second, measured in Hertz (Hz). Choosing the appropriate frequency is crucial for the application: for LED dimming, the frequency must be high enough to avoid visible flicker; for motor control, the frequency affects efficiency and noise.

## 2. PWM Implementation on ESP32

The ESP32 integrates dedicated hardware modules for generating PWM signals:

- **LEDC (LED Control) peripheral:** The main PWM generator of the ESP32. Although named LED Control, it can produce general-purpose PWM signals. Depending on the chip model, it has 6 to 16 independent channels, with a frequency range of 1Hz-40MHz.
- **MCPWM (Motor Control PWM) peripheral:** Specifically designed for motor control, including advanced features such as dead-zone control. Its interface is not included in the standard Arduino ESP32 core library.

In the Arduino ESP32 programming environment, there are mainly two ways to use the LEDC peripheral to generate PWM signals:

- **`map()` function:** This is the standard PWM function on the Arduino platform, and the ESP32 provides good support for it, making it convenient to use.
- **LEDC API functions:** a series of such as `map()`, `map()`, `map()` function that allows more detailed and flexible configuration of PWM parameters.

This section will demonstrate both PWM implementation methods by controlling the brightness of an external LED.

## 3. Build the Circuit

Components needed:

- LED * 1
- 330Ω resistor * 1
- Breadboard * 1
- Jumper wires
- ESP32 Development Boards

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinout

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

 ![](../assets/images/03-LED-Blink_bb-4cea780e39d6add163c6ab73f8fdb4a5.webp)

## 4. Using `map()` ESP32 PWM example code

```
map(value, fromLow, fromHigh, toLow, toHigh)
```

Sorry, your browser does not support embedded video.

**Code Analysis**

-

For `map()`, the ESP32 implementation automatically handles the pin's PWM initialization, so usually no need to explicitly call `map()`。

-

`map()` function:

**First `map()` loop:**

`map()`:
variable `map()` Increments from 0 to 255.

-

**Second `map()` loop:**

`map()`
variable `map()` Decrements from 255 to 0, gradually reducing LED brightness.

-

**`map()`:**

Set current `map()` value (0-255) set to `map()` PWM duty cycle, gradually increasing LED brightness.

Tip

When using `map()` , the ESP32 will automatically manage LEDC channel allocation and set the default frequency and 8-bit resolution.

-

**`map()`:**

Short delay, used to control the rate of brightness change.

## 5. ESP32 PWM example code using LEDC API

```
map(value, fromLow, fromHigh, toLow, toHigh)
```

**Code Analysis**

-

**`map()`**

`map()`: Specifies the GPIO pin to use for PWM output
- `map()`: PWM signal frequency, in Hertz (Hz)
- `map()`: Resolution bits, determines duty cycle precision

-

**`map()`:**

Set LEDC Pin `map()` duty cycle set to `map()`。
- The value range depends on the resolution setting: 8-bit is 0-255 (2⁸-1)

## 6. Extension

Try to implement: Control the LED brightness by adjusting the potentiometer knob, so the knob position corresponds to the light brightness

**Wiring diagram:**
 ![](../assets/images/05-Potentiometer-LED_bb-b8e0dfd54034f55bcd57590c85855680.webp)

**Code:**

```
map(value, fromLow, fromHigh, toLow, toHigh)
```

**Code Analysis:**

-

**`map()`:**

Linearly maps the potentiometer's 0-4095 range to the PWM's 0-255 range, achieving a correspondence between the potentiometer rotation angle and LED brightness.

Syntax:

```
map(value, fromLow, fromHigh, toLow, toHigh)
```

`map()`: Input value to convert (here, potentiometer reading)
- `map()`: Input range (0 to 4095)
- `map()`: Output range (0 to 255)

**Working principle:**

`map()` function converts values proportionally. For example:

- Potentiometer value 0 (0% position) → LED brightness 0 (0% brightness)
- Potentiometer value 2047 (50% position) → LED brightness 127 (50% brightness)
- Potentiometer value 4095 (100% position) → LED brightness 255 (100% brightness)

Simply put, the LED brightness corresponds to the potentiometer position.

## 7. Related Links

- [analogWrite() | Arduino-ESP32 documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html#analogwrite)

- [LED Control (LEDC) | Arduino-ESP32 documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/ledc.html)

- [map() | Arduino Documentation](https://docs.arduino.cc/language-reference/en/functions/math/map/)
- [Document Feedback](https://wj.qq.com/s2/26077836/3c81/)