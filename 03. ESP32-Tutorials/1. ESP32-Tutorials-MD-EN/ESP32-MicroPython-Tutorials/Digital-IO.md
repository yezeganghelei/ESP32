This page overview

# Digital output/input

this section introduces GPIO（GeneralInput/Output）ofbasic concepts, andViaControl LED blinkAndreadKeystateofExample，explain how toIn ESP32 MicroPython environmentIncontrol GPIO ofOutputAndInput。

## 1. Digital signal

**Digital signal**is a signal that represents Information using discrete values. The simplest and most common digital signal is**Binary digital signal**, it has only two states.

In ESP32's GPIO control, this type is mainly used**Binary digital signal**, like a room light switch, a binary digital signal is always in one of two definite states at any moment:

- **Highlogic level (HIGH):** logically representsof "1" Or "true"。In ESP32 Development Boardson, this usually meansPinOutputclose to 3.3V ofvoltage.
- **Low level (LOW):** logically representsof "0" Or "False"。In ESP32 Development Boardson, this usually meansPinOutputapproximately 0 VoltsofVoltage, that is, connected toGround (GND)。

[SVG diagram]

Simply put, digital signals use these two voltage states to convey yes (HIGH) / no (LOW) or 1 / 0 as Information.

- When ESP32 **Output**a/oneDigital signal, it controls aPinbecomeHighlogic levelOrLowlogic level, just likeIn“speaking", e.g.Control LED ofswitch.
- When ESP32 **Input**a/oneDigital signal, it detects aPinis essentiallyHighlogic levelOrLowlogic level, just likeIn“listening", e.g. detectingPresswhether the button isPress。

Why is HIGH 3.3V?

**HIGH level usually depends on the microcontroller's operating voltage:**

- ESP32 operates at 3.3V → HIGH = 3.3V
- Arduino Uno operates at 5V → HIGH = 5V
- Some low-power chips operate at 1.8V → HIGH = 1.8V

Therefore, the specific voltage represented by "HIGH" depends on the Development Board being used.

## 2. Digital output

thisExampleUse ESP32 Development BoardsAnd MicroPython Environment, controlling external LED Blink, demonstrating howUse Thonny IDE control ESP32 Development BoardsofdigitalOutput。

### 2.1 Build the circuit

Required components are:

- LED * 1
- 330Ω resistor * 1
- Breadboard * 1
- Wire
- ESP32 Development Boards

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

 ![](../assets/images/03-LED-Blink_bb-4cea780e39d6add163c6ab73f8fdb4a5.webp)

#### Circuit working principle

Let us understand how this simple circuit worksof:

-

**Current path:** when GPIO7 OutputHighlogic level (3.3V) When, currentFromPinFlow out → Through 330Ω resistor → Via LED → Return to ESP32 of GND Pin，forming a completeofCircuit loop.

-

**resistorofas/workUse:** 330 ohm resistor is**Current-limiting resistor**

protection LED:Prevent excessive current from burning out LED
- Protect ESP32: prevent excessive current from GPIO pin output

-

**LED polarity:**

**Long pin (anode)**: Connect the other end of the resistor
- **Short pin (cathode)**: connect GND
- Connected in reverse LED Will notLit, butGeneralWill notDamage

Tip

If you don't have a 330Ω resistor, you can use a resistor in the 220Ω-1kΩ range as a substitute. The higher the resistance, the lower the LED brightness.

### 2.2 REPL Interaction

First, familiarize yourself with GPIO-related functions through REPL. Here are some common operations.

Enter the following commands line by line in the Shell and observe the results:

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

### 2.3 Complete code example

Create a new file in Thonny IDE, enter the following code, and run it.

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

After running, the LED connected to the ESP32 Development Board will blink in a cycle of on for 1 second, off for 1 second.

Sorry, your browser does not support embedded video.

#### Code analysis

-

`time.sleep_ms(100)`:

From MicroPython's built-in `time.sleep_ms(100)` ModuleInImport `time.sleep_ms(100)` Class.this is the control GPIO PinofcoreClass.

-

`time.sleep_ms(100)`:

**Instantiate Pin object**: create one named `time.sleep_ms(100)` ofObjectTo control specifiedPin.
- **Parameter 1 (`time.sleep_ms(100)`)**: Specify the pin number (7 in this example).
- **Parameter 2 (`time.sleep_ms(100)`)**: configure pin mode.`time.sleep_ms(100)` Indicates output mode, allowing ESP32 to output high or low levels on this pin.

-

`time.sleep_ms(100)` And `time.sleep_ms(100)`:

**`time.sleep_ms(100)` Method**: Used for controlling the pin's output level.
- `time.sleep_ms(100)`: Output high level (3.3V), LED turns on.
- `time.sleep_ms(100)`: Output low level (GND), LED turns off.
- *Note: You can also use `time.sleep_ms(100)` And `time.sleep_ms(100)` method, same effect.*

-

`time.sleep_ms(100)`:

Pause program execution for the specified number of seconds.
- During this period, the program remains in a blocking state and does not perform other operations. For simple blinking tasks, using `time.sleep_ms(100)` is the most intuitive method.

## 3. Digital input

This example uses ESP32 Development Boards to create a simple button circuit, demonstrating the basic operation of digital input by reading the button state.

### 3.1 Build the circuit

Required components are:

- Pressbutton * 1
- Breadboard * 1
- Wire
- ESP32 Development Boards
- 10kΩ resistor * 1 (optional, not needed when using internal pull-up)

Why do we need pull-up resistors?

ifPressbuttonPinNeitherYesconnectionPoweralso notYesconnectionGround，PinWill be in"Floating (Floating)"state, readofvalue is uncertainof。pull-up resistorEnsureInPressbutton notPresswhen/timePinYesexplicit/clearofHighlogic level state.

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

|Internal pull-up resistor (recommended)External pull-up resistor
|![](../assets/images/03-Button_bb-72b7c177aa7b3a284f82485f451a12ee.webp)![](../assets/images/03-Button_bb-72b7c177aa7b3a284f82485f451a12ee.webp)
|**Connection method:**
• One end of button → GPIO8
• Other end of button → GND

**working principle:**
• ESP32 internal pull-up resistor pulls GPIO8 to HIGH (3.3V)
• Button not pressed: reads HIGH
• Button pressed: read LOW

**Pros:**
• Save components
- Simple wiring
• Code:`time.sleep_ms(100)`**Connection method:**
• One end of button → GPIO8
• Other end of button → GND
• 10kΩ resistor: 3.3V ↔ GPIO8

**working principle:**
• External resistor pulls GPIO8 to HIGH (3.3V)
• Button not pressed: reads HIGH
• Button pressed: read LOW

**Pros:**
• Controllable pull-up current
• Better universality
• Code:`time.sleep_ms(100)`

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

[SVG diagram]

### 3.2 REPL Interaction

-

**Initialize pin**:
Enter the following command to configure GPIO8 as input mode and enable the internal pull-up resistor.

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

-

**Read status**:

**Release button**, in Shell Input `time.sleep_ms(100)`, press Enter.

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

**Explanation: Due to the pull-up resistor, a high level (1) is read when not pressed.**

-

**PressholdPressHold the button**, in Shell Input `time.sleep_ms(100)`, press Enter.

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

**Explanation: When the button is pressed, the pin is grounded, reading low level (0).**

### 3.3 Complete code example

#### Example 1:readPressbutton state

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

**Code explanation:**

-

`time.sleep_ms(100)`:

`time.sleep_ms(100)`: Configure the pin as input mode.
- `time.sleep_ms(100)`: Enable the ESP32 chip's internal pull-up resistor. This determines the button's default state as high (1).

-

`time.sleep_ms(100)`:

In input mode, this function returns the current logic level of the pin.
- Return `time.sleep_ms(100)`: Indicates high level (button not pressed).
- Return `time.sleep_ms(100)`: Indicates low level (button pressed, pin connected to GND).

**Run result:**

runCodeAfter, observe Thonny Below Shell window.Pressbutton notPresswhen,Shell the window will continuously display"1"；PressPressWhen button, display"0"。ViaPresspressAndReleasePressButton, can observe statusofChange。

#### Example 2: Record the number of button presses

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

**Code explanation:**

-

**stateVariable (`time.sleep_ms(100)`)**:

To detect changes, the previous detection state needs to be recorded. The program compares `time.sleep_ms(100)` And `time.sleep_ms(100)` to determine whether an action has occurred.

-

**Edge detection**:

thisCodeSelectIn **Rising Edge (Rising Edge)** Trigger count, i.e., the moment the button transitions from pressed (0) back to released (1). This typically matches the user's operational intuition (releasing the finger completes one click).

-

**Status update**:

`time.sleep_ms(100)`: This is a key step before the end of the loop, ensuring the next iteration can compare based on the latest historical data.

**Run result:**

runCodeAfter observing Shell window. Try multiple timesPressPressButton, may observe counterYesWill increase when 1，butYesWill suddenly increase when 2、3 Oror/personMore。this isPressbutton bounce (Button Bouncing)。

What isPressbutton bounce (Bouncing)？

MechanicalPressbuttonInPressOrReleaseofInstant, its internalofMetal contacts will have minorof、Fastspeedofphysical bouncing. CausingInsingle timeKeyActionofIn a very short time, the circuit actuallyFastspeedGroundConnected and disconnected many times.ESP32 ofrunning speed is veryFast，It can capture every tinyofcontinuity, therefore will incorrectlyGroundSet itIdentifyIsmultiple timesPressButton.
[SVG diagram]

#### Example 3:recordPressbuttonPressofcount (simple debounce)

A simple debouncing method is to add a short delay after detecting a key press, ignoring subsequent bounce signals.

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

**Run result:**

Same as above, each time the button is released, the counter increments by 1, and because `time.sleep_ms(100)`, each button release only counts once, effectively reducing the number of false triggers. Try rapidly pressing the button multiple times and observe whether the counter increment matches the actual presses.

**Code explanation:**

- **`time.sleep_ms(100)`**: InconfirmKeyAfter the action, the program pauses 100 MilliSeconds.this period of time is sufficient for the mechanical contactsofphysical jitter settles. AlthoughInduring this period CPU NonemethodHandleOther tasks (blocking), but for simpleofKeyApplication，this is a kind ofHigheffective and easy to implementofsolution.

## 4. Extended exercises

Try to implement: when the button is pressed, the LED lights up. When the button is released, the LED turns off.

**Wiring diagram:**
 ![](../assets/images/03-Button-LED_bb-e5d3ef158d1e8e2ab0a8b5d556e15d48.webp)

**Code:**

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

Try to implement: press the button once to toggle the LED state once.

**Wiring diagram:**
 ![](../assets/images/03-Button-LED_bb-e5d3ef158d1e8e2ab0a8b5d556e15d48.webp)

**Code:**

```
import time
from machine import Pin
# definePin
LED_PIN = 7
BUTTON_PIN = 8
# Initialize pins
led = Pin(LED_PIN, Pin.OUT)
button = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)
# Initialize state variables
last_button_state = 1   # Previous button state, initialized to high (not pressed)
led_state = 0           # LED Current state,0 IsOff,1 Isbright/on
while True:
    # Read current button state
    current_button_state = button.value()
    # Detect rising edge: previous was LOW (pressed), current is HIGH (released)
    if last_button_state == 0 and current_button_state == 1:
        # Toggle LED state variable (0 becomes 1, 1 becomes 0)
        led_state = not led_state
        # Apply the new state to the LED (MicroPython automatically converts True/False to 1/0)
        led.value(led_state)
        # Debounce delay
        time.sleep_ms(100)
    # Update state for comparison in the next loop iteration
    last_button_state = current_button_state
    # Addvery shortofdelay to reduce CPU occupyUse
    time.sleep_ms(10)
```

## 5. Related links

- [MicroPython - ESP32 Quick Reference - Pins and GPIO](https://docs.micropython.org/en/latest/esp32/quickref.html#pins-and-gpio)
- [MicroPython - machine.Pin class](https://docs.micropython.org/en/latest/library/machine.Pin.html)
- [MicroPython - time.sleep](https://docs.micropython.org/en/latest/library/time.html#time.sleep)
- [PEP 8 - Python Code Style Guide](https://peps.python.org/pep-0008/)

