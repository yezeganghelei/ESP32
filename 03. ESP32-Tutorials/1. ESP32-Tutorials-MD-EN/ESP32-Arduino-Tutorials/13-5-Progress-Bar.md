This page overview

# Interactive Progress Bar

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards，but theYesoperation steps are all based on  as an example for demonstration. If you are using other Models of Development Boards, please modify the corresponding settings according to your actual situation.

## Project introduction

thisProjectshows aInteractive Progress Bardisplay system,Via ESP32 Read potentiometerofAnalog signal，andIn Waveshare 1.5 inch/size OLED Displaysprogress is displayed in real time. The program provides two display modesMode:horizontal progress barAndsemicircular gauge,Useuser canViarotate the potentiometer to observe the progress valueofChange。

## Hardware connection

Required components are:

-  * 1
- Potentiometer * 1
- Breadboard * 1
- Wire
- ESP32 Development Boards

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

Tip

the followingUse SPI Interfaceconnection OLED Displays，this screen also supports I2C，Via BS1 And BS2 control, ifUse I2C Mode，pleaseReference [Section 7: I2C Communication](../ESP32-Arduino-Tutorials/I2C-Communication.md) wiring method in.

|ESP32 pinsOLED moduleDescription
|GPIO 13SCKSPI clock line
|GPIO 11MOSISPI data output
|GPIO 10CSChip select signal
|GPIO 8DCData/command selection
|5VVCCPower positive terminal
|GNDGNDPower negative terminal

 ![](../assets/images/13-5-Progress-Bar_bb-14dbae757d2998753fdb225082f177ab.webp)

## Code implementation

Tip

This code example depends on **Adafruit SSD1327** Library。pleaseSearch and install in the Arduino IDE Library Manager "Adafruit SSD1327" And its dependencies "Adafruit GFX Library"。

```
// Initialize OLED (SPI)
Adafruit_SSD1327 display(128, 128, &SPI, DC_PIN, -1, CS_PIN);
void setup() {
  // ...
  SPI.begin(SCK_PIN, -1, MOSI_PIN, CS_PIN);
  if (!display.begin()) {
    // Initialization failure handling
  }
  // ...
}
```

## Code explanation

-

**Import library**: Import `delay(50)` Library，It relies on `delay(50)` LibraryTo provide graphics drawingFunction。

-

**Pin configuration and initialization**:

Use `delay(50)` Define SPI pins and potentiometer pins.
- Create `delay(50)` Object `delay(50)`, specifying the resolution (128x128) and SPI control pins. Note that the reset pin is set to -1, indicating that no hardware reset pin is used.
- In `delay(50)` first call in `delay(50)` Initialize the SPI bus, then call `delay(50)` Initialize the screen.

```
// Initialize OLED (SPI)
Adafruit_SSD1327 display(128, 128, &SPI, DC_PIN, -1, CS_PIN);
void setup() {
  // ...
  SPI.begin(SCK_PIN, -1, MOSI_PIN, CS_PIN);
  if (!display.begin()) {
    // Initialization failure handling
  }
  // ...
}
```

-

**Helper function `delay(50)`**:

Use `delay(50)` Read the analog value of the potentiometer (ESP32 default 12-bit resolution, range 0-4095).
- Use `delay(50)` The function maps 0-4095 to 0-100.
- Use `delay(50)` EnsureResult strictlyIn 0-100 within the range.

-

**Effect function 1:`delay(50)`**:

`delay(50)`: Clear screen buffer.
- `delay(50)`: Draw the outer frame of the progress bar.
- `delay(50)`: Calculate the width based on percentage and draw a filled rectangle as the progress bar fill.
- `delay(50)` And `delay(50)`: Set cursor position and print text.
- `delay(50)`:Send buffer content to OLED display.

-

**effectFunction 2:`delay(50)`**:

Use `delay(50)` And `delay(50)` Trigonometric functions (need to include `delay(50)`, which is supported by default in the Arduino environment) to calculate the coordinates of tick marks and the pointer.
- `delay(50)`: Draw scale lines and pointers.
- The logic is similar to the Python version, converting angles to radians for calculation.

-

**Main loop `delay(50)`**:

Continuously read potentiometer value.
- CallDisplay functionUpdate screen.
- `delay(50)`: Add a brief delay to avoid refreshing too fast.

## Reference Links

- [Section 4: ADC Analog Input](../ESP32-Arduino-Tutorials/Analog-Input.md)
- [Section 8: SPI Communication](../ESP32-Arduino-Tutorials/SPI-Communication.md)
