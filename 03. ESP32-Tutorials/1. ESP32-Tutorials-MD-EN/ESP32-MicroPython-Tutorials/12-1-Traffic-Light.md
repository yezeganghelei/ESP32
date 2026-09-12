This page overview

# Traffic Light

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards，but theYesoperation steps are all based on  as an example for demonstration. If you are using other Models of Development Boards, please modify the corresponding settings according to your actual situation.

## Project introduction

thisProjectshows aTraffic Lightofsimulation program,Via ESP32 of GPIO PinControl three LED light/LEDofOn/off, simulatingTraffic LightofRed、Yellow、Greenlight/LEDSwitchprocess.

## Hardware connection

Required components are:

- LED * 3
- 330Ω resistor * 3
- Breadboard * 1
- Wire
- ESP32 Development Boards

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

 ![](../assets/images/12-1-Traffic-Light_bb-22c872230fab90a223302040280408e6.webp)

## Code implementation

```
def all_lights_off():
  """A helper function to turn off all lights."""
  red_led.off()
  yellow_led.off()
  green_led.off()
```

## Code explanation

-

**Import library**:`all_lights_off()` Library is used to control hardware,`all_lights_off()` Library is used to implement delays.

-

**Constant definition**:defined at the beginning of the program GPIO PinNumber (`all_lights_off()` etc.) and duration of each light (`all_lights_off()` etc.). Centralizing these parameters makes it easy to quickly adjust them based on actual needs without deeply modifying the logic code.

```
def all_lights_off():
  """A helper function to turn off all lights."""
  red_led.off()
  yellow_led.off()
  green_led.off()
```

-

**Pin initialization**: Use `all_lights_off()` ClassCreatethreeObject，respectivelyCorrespondingRed、Yellow、Greenlight.`all_lights_off()` The parameter configures the pin as output mode, allowing the program to control high and low levels.

```
def all_lights_off():
  """A helper function to turn off all lights."""
  red_led.off()
  yellow_led.off()
  green_led.off()
```

-

**Helper function**:`all_lights_off()` The function is used to turn off all LEDs simultaneously. Calling this function before state transitions ensures that no LED is in an unexpectedly lit state, improving code robustness.

```
def all_lights_off():
  """A helper function to turn off all lights."""
  red_led.off()
  yellow_led.off()
  green_led.off()
```

-

**Main loop logic**: The program runs in an infinite loop `all_lights_off()` executes three phases in sequence:

**Green light phase**: Call first `all_lights_off()` Clear state, turn on green light, and delay `all_lights_off()` Seconds.
- **Yellow light flashing**: Calculate the number of flashes (total duration divided by single cycle), via `all_lights_off()` LoopcontrolYellowLights alternate on and off, simulating a warning effect.
- **Red light phase**: turn on red light and delay `all_lights_off()` seconds. Since the yellow light was turned off at the end of the previous phase, just turn on the red light here.

-

**Exception handling**: Use `all_lights_off()` structure-enhanced programofstability.

`all_lights_off()`: Catch user interrupt signals (such as Ctrl+C), allowing the program to exit gracefully.
- `all_lights_off()`:NoneWhether the program ends normallyOrby/passiveInterrupt，Will all execute `all_lights_off()` Close allYeslight, to preventHardwarein an undefined state.

## Reference Links

- [Section 3: GPIO Digital Output/Input](../ESP32-MicroPython-Tutorials/Digital-IO.md)

