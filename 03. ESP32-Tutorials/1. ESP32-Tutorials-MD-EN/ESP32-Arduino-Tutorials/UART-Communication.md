This page overview

# Serial communication (UART)

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards，but theYesoperation steps are all based on  as an example for demonstration. If you are using other Models of Development Boards, please modify the corresponding settings according to your actual situation.

UART (Universal Asynchronous Receiver/Transmitter)is a kind ofHardwareInterfacecircuit,Usefor implementingAsynchronousserial communication. OftenSeeApplicationinclude:AndSensors/Modulecommunication,Development BoardAndbetween computersofdata transception (e.g. printingLog、debugInformation）etc.。

based on UART ofserial communication hasYesthe followingFeatures:

- **Asynchronous communication**: The sending and receiving devices do not need to share a clock signal; instead, they synchronize data transmission through a pre-agreed baud rate.
- **Serial transmission**: Data is sent bit by bit, rather than transmitting multiple bits in parallel.
- **Full-duplex**: Can perform send and receive operations simultaneously.

---

UART is **Asynchronous** Communication means there is no shared clock line. To achieve correct data transmission and reception, they agree to use the same **Baud rate** And **Data frame format**。

**Baud Rate (Baud Rate)** represents transmission per secondofdata bits (bps，bits per second）。both communicating parties mustUsesameofBaud rateto correctly transmit data. OftenSeeofBaud rateYes 9600 And 115200。

Each **UART data frame** contains the following parts:

- **Start Bit (Start Bit)**:1 bit, always 0，indicates start of data transmission
- **Data Bits (Data Bits)**: Usually 5-9 bits, commonly 8 bits, containing the actual data to be transmitted
- **Parity Bit**: Optional, for error detection
- **Stop Bits (Stop Bits)**:1-2 bit, always 1，indicates end of data transmission

[SVG diagram]

---

UART communication requires two core signal lines:

- **TX (Transmit)**:sendData line
- **RX (Receive)**:receiveData line
- **GND (Ground)**: both communicating partiesof“together/commonReferencepoint”,EnsurevoltageSignalcan be correctly interpreted.
- **Connection method**:cross connection is needed between two devices, i.e.device A of TX connect device B of RX，device A of RX connect device B of TX。In addition,two devicesMust share common ground（connection GND），with/byEnsureSignallogic levelYesstableofReferencepoint.

[SVG diagram]

---

## 1. UART in ESP32

ESP32 chips usuallyYesTwoOrMore UART controller.Each UART Controllers can be configured independentlyBaud rate、Data bit length, bit order, stop bit count, parity bitetc.Parameter。

In the Arduino environment,we canVia `Serial.begin (baud rate)`、`Serial.begin (baud rate)` and other objects to use them.

- **`Serial.begin (baud rate)`** Is the default serial port, usually connected toDevelopment Boardon/upof USB USB-to-serial chip. If notYesequipped with USB USB-to-serial chip, itcan also be done viaenable USB CDC on boot points to native USB。NoneRegardless of the situation,`Serial.begin (baud rate)` can be used for communication with the computer's serial monitor, and are commonly used interfaces for uploading code and debugging.

- **`Serial.begin (baud rate)` etc.** The other serial ports are additional hardware UARTs that can be assigned to almost any free GPIO for connecting external devices.

- Except `Serial.begin (baud rate)` And `Serial.begin (baud rate)` Additionally, some ESP32 Models (such as ESP32-S3) also support `Serial.begin (baud rate)` and other More serial ports. The number of UARTs supported by various models can be found in the chip datasheet or [ESP32 Series Product Introduction Table](https://products.espressif.com/api/user/file/Espressif_SoC_Product_Portfolio.pdf)。

this design allows us to simultaneouslyUse `Serial.begin (baud rate)` Debugging with computer, printing logs, while using `Serial.begin (baud rate)` Other UARTs communicate with the module independently without interfering.

Information

In the Arduino environment,`Serial.begin (baud rate)` The pointing varies depending on the Development Board type and configuration; please note:

- **Development Board with USB-to-serial chip (such as the classic ESP32):**
`Serial.begin (baud rate)` usuallyCorrespondingunderlying **UART0**，defaultof TX/RX dedicated/specializedUsefor communication with computer.

- **Development Boards with native USB functionality (such as ESP32-S3/S2/C3):**

If equipped with USB-to-serial chip,`Serial.begin (baud rate)` Corresponding **UART0**，defaultof TX/RX dedicated/specializedUsefor communication with computer

When USB CDC is enabled,`Serial.begin (baud rate)` It will communicate directly through the USB interface, independent of hardware UART0. If there is no USB-to-serial chip, you can use the labeled RX/TX pins with `Serial.begin (baud rate)`(UART0)。

## 2. Example 1: Control LED via serial monitor

thisExampleWill demonstrate UART ofclassicApplication:ViaComputer sends commands to control ESP32 ofHardware。we willVia Arduino IDE ofserial portMonitorDevice sends "on" Or "off" string, to light upOrTurn offconnectionIn ESP32 on/upof LED。

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

### 2.2 Code

```
#define UART1_RX_PIN 1  // Define the RX pin for UART1
#define UART1_TX_PIN 2  // Define the TX pin for UART1
const int ledPin = 7;
void setup() {
  // Startdefault serial port,Usefor debuggingOutputto the computer
  Serial.begin(115200);
  // while(!Serial){};
  // Start Serial1 and specify RX and TX pins for inter-device communication
  Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  pinMode(ledPin, OUTPUT);  // configuration LED PinIsOutputMode
  Serial.println("Receiver Ready. Waiting for commands...");
}
void loop() {
  // Check if data was received from the UART1 serial port
  if (Serial1.available()) {
    char command = Serial1.read();  // Read one byte (character)
    // Control LED based on received command
    if (command == '1') {
      // Turn on LED when '1' is received
      digitalWrite(ledPin, HIGH);
      Serial.println("Received: 1 -> LED ON");
    } else if (command == '0') {
      // Turn off LED when '0' is received
      digitalWrite(ledPin, LOW);
      Serial.println("Received: 0 -> LED OFF");
    }
  }
}
```

### 2.3 Code analysis

- **`Serial.begin (baud rate)`**: Initialize `Serial.begin (baud rate)`, and set the baud rate to 115200. The serial monitor's baud rate also needs to be set to this value.
- **`Serial.begin (baud rate)`**: Check if there is data in the serial receive buffer. If greater than 0, it means the computer sent a new message.
- **`Serial.begin (baud rate)`**: Reads characters from the serial buffer until a newline character \n or timeout is encountered, and combines the read characters into a `Serial.begin (baud rate)` object. This method is suitable for receiving commands sent from the serial monitor, terminated with Enter.
- **`Serial.begin (baud rate)`**: When we send text from the serial monitor and press Enter, in addition to the text itself, a newline character is typically also sent (`Serial.begin (baud rate)`）OrCarriage return (`Serial.begin (baud rate)`）。`Serial.begin (baud rate)` FunctionWill moveExceptstring beginning and endofthese emptyWhitecharacter,Ensure `Serial.begin (baud rate)` Such comparison can succeed.
- **`Serial.begin (baud rate)`**: based on the cleaned `Serial.begin (baud rate)` string content, execute the correspondingof `Serial.begin (baud rate)` operations to control the LED and print feedback Information to the serial monitor.

### 2.4 Run results

- Open the serial monitor and set the baud rate to 115200, ensuring it matches the baud rate set in the code.

![](../assets/images/06-Set-Baud-Rate-27e01dd5321df2c5689e63ac8a4833fc.webp)

- In the serial monitor, type 'on' and press Enter to turn on the LED; type 'off' and press Enter to turn off the LED.

Sorry, your browser does not support embedded video.

## 3. Example 2:ESP32 serial communication between

This example will show how to use ESP32's additional hardware serial port (`Serial.begin (baud rate)`）implementTwo ESP32 Development Boardsbetweenofcommunication. We willUsea piece ofDevelopment BoardconnectionofPressButton, to control anotherDevelopment BoardConnect onof LED。

### 3.1 Build the circuit

Required components are:

- LED * 1
- 330Ω resistor * 1
- Breadboard * 2
- Pressbutton * 1
- Wire
- ESP32 Development Boards * 2

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

 ![](../assets/images/06-UART_bb-3626aa0c6695da8faba26bc6a3d213bc.webp)

|Sender (Development Board A)Receiver (Development Board B)Description
|GPIO 11 (RX)GPIO 2 (TX)dataFrom B send to A
|GPIO 12 (TX)GPIO 1 (RX)dataFrom A send to B
|GNDGND**Must share common ground**，ensureSignalstable

### 3.2 Code

#### 3.2.1 Sender Code (ESP32 Development Boards A)

Upload this code to the ESP32 Development Boards connected to buttons.

```
#define UART1_RX_PIN 1  // Define the RX pin for UART1
#define UART1_TX_PIN 2  // Define the TX pin for UART1
const int ledPin = 7;
void setup() {
  // Startdefault serial port,Usefor debuggingOutputto the computer
  Serial.begin(115200);
  // while(!Serial){};
  // Start Serial1 and specify RX and TX pins for inter-device communication
  Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  pinMode(ledPin, OUTPUT);  // configuration LED PinIsOutputMode
  Serial.println("Receiver Ready. Waiting for commands...");
}
void loop() {
  // Check if data was received from the UART1 serial port
  if (Serial1.available()) {
    char command = Serial1.read();  // Read one byte (character)
    // Control LED based on received command
    if (command == '1') {
      // Turn on LED when '1' is received
      digitalWrite(ledPin, HIGH);
      Serial.println("Received: 1 -> LED ON");
    } else if (command == '0') {
      // Turn off LED when '0' is received
      digitalWrite(ledPin, LOW);
      Serial.println("Received: 0 -> LED OFF");
    }
  }
}
```

#### 3.2.2 Receiver Code (ESP32 Development Boards B)

Upload this code to the ESP32 Development Boards connected to LEDs.

```
#define UART1_RX_PIN 1  // Define the RX pin for UART1
#define UART1_TX_PIN 2  // Define the TX pin for UART1
const int ledPin = 7;
void setup() {
  // Startdefault serial port,Usefor debuggingOutputto the computer
  Serial.begin(115200);
  // while(!Serial){};
  // Start Serial1 and specify RX and TX pins for inter-device communication
  Serial1.begin(9600, SERIAL_8N1, UART1_RX_PIN, UART1_TX_PIN);
  pinMode(ledPin, OUTPUT);  // configuration LED PinIsOutputMode
  Serial.println("Receiver Ready. Waiting for commands...");
}
void loop() {
  // Check if data was received from the UART1 serial port
  if (Serial1.available()) {
    char command = Serial1.read();  // Read one byte (character)
    // Control LED based on received command
    if (command == '1') {
      // Turn on LED when '1' is received
      digitalWrite(ledPin, HIGH);
      Serial.println("Received: 1 -> LED ON");
    } else if (command == '0') {
      // Turn off LED when '0' is received
      digitalWrite(ledPin, LOW);
      Serial.println("Received: 0 -> LED OFF");
    }
  }
}
```

### 3.3 Code analysis

#### 3.3.1 Commonalities between the two code examples

- **`Serial.begin (baud rate)` / `Serial.begin (baud rate)`**: Use macro definitions to specify `Serial.begin (baud rate)` RX and TX pins. This makes the code more readable and easier to modify.
- **`Serial.begin (baud rate)`**: Both boards started the default `Serial.begin (baud rate)` ports, so they can be connected to two computers (or two serial port tools on the same computer) respectively, printing debug Information to facilitate observing the communication process.
- **`Serial.begin (baud rate)`**:

This is the core of this example. It initializes **`Serial.begin (baud rate)`** channel. Two Development Boards communicate through their respective **`Serial.begin (baud rate)`** Channel communication.
- `Serial.begin (baud rate)`: This is the baud rate for communication between two ESP32s,**must maintainConsistent**。
- `Serial.begin (baud rate)`: This is the standard serial configuration (8 data bits, no parity, 1 stop bit).

`Serial.begin (baud rate)`: 8-bit data length (optional 5, 6, 7 bits)
- `Serial.begin (baud rate)`: Noneparity check (optional even parity E、Odd parity O）
- `Serial.begin (baud rate)`: 1 stop bit (optional 2 bits)

- `Serial.begin (baud rate)`: Set `Serial.begin (baud rate)` Bind to the defined GPIO pin.

#### 3.3.2 Sender (ESP32 Development Boards A)

- **`Serial.begin (baud rate)`**: This check is used to detect button status**Change**(from pressed to released, or from released to pressed), ensuring data is sent only once when the state changes, rather than continuously.
- **`Serial.begin (baud rate)`**: When the button is pressed (state changes to`Serial.begin (baud rate)`), via `Serial.begin (baud rate)` Send a single character `Serial.begin (baud rate)` to the receiver.
- **`Serial.begin (baud rate)`**: When the button is released (state changes to`Serial.begin (baud rate)`), send `Serial.begin (baud rate)`。

#### 3.3.3 Receiver (ESP32 Development Boards B)

- **`Serial.begin (baud rate)`**: In the main loop, continuously check `Serial.begin (baud rate)` whether there is data in the receive buffer.
- **`Serial.begin (baud rate)`**: If there is data, read one byte (character) and store it in `Serial.begin (baud rate)` Variable.
- **`Serial.begin (baud rate)`**: Determine the received character. If it is `Serial.begin (baud rate)`, turn on the LED; if it is `Serial.begin (baud rate)`, turn off the LED. At the same time, through its own `Serial.begin (baud rate)` Print received Information and executed actions to the serial port for debugging convenience.

### 3.4 Run results

- Upload the two sets of code to two separate ESP32 Development Boards.
- You can connect both boards to the computer using two USB cables and open two serial monitor windows, corresponding to the COM ports of each board.
- Press **Sender (ESP32 Development Boards A)** button, you will observe:

ESP32 A ofserial portMonitorthe device prints out `Serial.begin (baud rate)`。
- **Receiver (ESP32 Development Boards B)** on/upof LED **Light up**。
- ESP32 B's serial monitor prints out `Serial.begin (baud rate)`。

- Release button，You will observe:

ESP32 A ofserial portMonitorthe device prints out `Serial.begin (baud rate)`。
- **Receiver (ESP32 Development Boards B)** on/upof LED **Turn off**。
- ESP32 B's serial monitor prints out `Serial.begin (baud rate)`。

Sorry, your browser does not support embedded video.

## 4. Extended Reading

### Why does communication still work normally after enabling USB CDC, even when the code and serial monitor baud rate settings do not match?

The core reason is that at this point, the Development Board and the computer are using **Native USB communication**, it simulates a **Virtual serial port**, instead of traditional UART serial communication. In the USB communication protocol, the concept of "baud rate" is invalid.

In short:

- the communication method has changed:YouUseofdoes not rely on a fixed rateof UART Signal，It transmitsofIs a data packet, its speedBy USB negotiated by the protocol itself. Data transmission speed is extremelyFast，far exceedsGeneralBaud rate。
- Baud rate setting is ignored: the code's `Serial.begin (baud rate)` and the monitor's baud rate settings are ignored. This function is kept mainly for backward compatibility with old code.
- suitableUseObject:thisPhenomenononly appearsInsupports native USB ofDevelopment BoardOn (such as ESP32-S2/S3, Leonardo, Zero etc.）。for Arduino Uno/Nano thisClassUseindependent USB USB-to-serial chipofboard,Baud ratemust strictly match.

## 5. Related links

- [Serial (UART) | Arduino-ESP32 documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/serial.html)
- [Universal Asynchronous Receiver-Transmitter (UART) | Arduino Documentation](https://docs.arduino.cc/learn/communication/uart/)
- [Serial | Arduino Documentation](https://docs.arduino.cc/language-reference/en/functions/communication/serial/)
- [String() | Arduino Documentation](https://docs.arduino.cc/language-reference/en/variables/data-types/stringObject/)
- [String length() and trim() Commands | Arduino Documentation](https://docs.arduino.cc/built-in-examples/strings/StringLengthTrim/)
- [trim() | Arduino Documentation](https://docs.arduino.cc/language-reference/en/variables/data-types/stringObject/Functions/trim/)
- [Serial.write() | Arduino Documentation](https://docs.arduino.cc/language-reference/en/functions/communication/serial/write/)
- [USB CDC | Arduino-ESP32 documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/api/usb_cdc.html)
- [USB CDC On Boot | Arduino-ESP32 documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/guides/tools_menu.html?#usb-cdc-on-boot)
