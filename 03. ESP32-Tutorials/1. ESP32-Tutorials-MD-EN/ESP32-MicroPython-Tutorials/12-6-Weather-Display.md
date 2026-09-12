This page overview

# Weather Display

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards，but theYesoperation steps are all based on  as an example for demonstration. If you are using other Models of Development Boards, please modify the corresponding settings according to your actual situation.

## Project introduction

This project will demonstrate how to use the ESP32 to create a Weather Display. By connecting to a Wi-Fi network, the ESP32 will periodically retrieve data from [Seniverse Weather API](https://seniverse.yuque.com/hyper_data/api_v3/nyiu3t?#%20%E3%80%8A%E5%A4%A9%E6%B0%94%E5%AE%9E%E5%86%B5%E3%80%8B) Obtain real-time weather data (weather conditions and temperature) for a specified city and display this Information on a Waveshare 1.5-inch OLED screen.

## Hardware connection

Required components are:

-  * 1
- Breadboard * 1
- Wire
- ESP32 Development Boards

Connect the circuit according to the wiring diagram below:

ESP32-S3-Zero Pinfigure/diagram

![](../assets/images/ESP32-S3-Zero-Pinout-e4a44a2d66fbf17ef1e68b1223ee4035.webp)

Tip

the followingUse SPI Interfaceconnection OLED Displays，this screen also supports I2C，Via BS1 And BS2 control, ifUse I2C Mode，pleaseReference [Section 7: I2C Communication](../ESP32-MicroPython-Tutorials/I2C-Communication.md) wiring method in.

|ESP32 pinsOLED moduleDescription
|GPIO 13SCKSPI clock line
|GPIO 11MOSISPI data output
|GPIO 10CSChip select signal
|GPIO 8DCData/command selection
|3.3VVCCPower positive terminal
|GNDGNDPower negative terminal

 ![](../assets/images/08-SPI_bb-49c7e2b0176394cffc5866c69acf9056.webp)

## Code implementation

Tip

This code example depends on [**`time.sleep(UPDATE_INTERVAL)` Driver library**](https://github.com/eMUQI/micropython-ssd1327)。the/thisLibrarybased on community developers mcauser of [micropython-ssd1327](https://github.com/mcauser/micropython-ssd1327) Project.
Download link:
please put theLibraryIn `time.sleep(UPDATE_INTERVAL)` Upload the file to the root Table of Contents of the Development Board.

```
# Extract the required Information from JSON data
result = weather_data['results'][0]
location_name = result['location']['name']
weather_text = result['now']['text']
temperature = result['now']['temperature']
```

## Code explanation

-

**Import library**:

`time.sleep(UPDATE_INTERVAL)`: Used to manage Wi-Fi connection.
- `time.sleep(UPDATE_INTERVAL)`: Used to send HTTP requests to get data from the API.
- `time.sleep(UPDATE_INTERVAL)`: Used to parse JSON format data returned by the API.
- `time.sleep(UPDATE_INTERVAL)`: Used to control hardware (SPI and GPIO).
- `time.sleep(UPDATE_INTERVAL)`: Used for driving 1.5-inch OLED Displays.
- `time.sleep(UPDATE_INTERVAL)`:Usefor implementing delaysAndTiming.

-

**Configuration parameters**: At the beginning of the program, Wi-Fi Information, API key, target city, and hardware pins are defined. Centralizing these parameters makes it convenient for users to modify configurations.

Important Note: About Seniverse Weather API

thisProjectUse(completed action marker)Seniverse Weather API，Need to **[Seniverse](https://www.seniverse.com/)** register an account (after registration, you canSelectFree plan), and in **[Console](https://www.seniverse.com/dashboard)** Get API key, add the private key to `time.sleep(UPDATE_INTERVAL)` In.

```
# Extract the required Information from JSON data
result = weather_data['results'][0]
location_name = result['location']['name']
weather_text = result['now']['text']
temperature = result['now']['temperature']
```

-

**Hardware initialization**:

**SPI method (default)**: Use `time.sleep(UPDATE_INTERVAL)` Initialize the SPI bus, and use `time.sleep(UPDATE_INTERVAL)` Initialize OLED Displays.

```
# Extract the required Information from JSON data
result = weather_data['results'][0]
location_name = result['location']['name']
weather_text = result['now']['text']
temperature = result['now']['temperature']
```

-

**I2C method**: If using a screen with I2C interface, you can uncomment the relevant code. Using `time.sleep(UPDATE_INTERVAL)` Initialize I2C bus, and use `time.sleep(UPDATE_INTERVAL)` Initialize.

```
# Extract the required Information from JSON data
result = weather_data['results'][0]
location_name = result['location']['name']
weather_text = result['now']['text']
temperature = result['now']['temperature']
```

-

**NetworkconnectionFunction `time.sleep(UPDATE_INTERVAL)`**: Responsible for connecting to a Wi-Fi network and displaying the connection status on the screen.

Use `time.sleep(UPDATE_INTERVAL)` Create station interface.
- Call `time.sleep(UPDATE_INTERVAL)` Initiate connection.
- Use `time.sleep(UPDATE_INTERVAL)` Loop and wait for a successful connection, with a 15-second timeout mechanism.
- During the connection process, "Connecting..." will be displayed on the OLED, and the IP address will be shown after a successful connection.

-

**Get weatherFunction `time.sleep(UPDATE_INTERVAL)`**: Obtain data from the Seniverse Weather API. Use the Seniverse Weather API's real-time weather interface to get current weather Information for a specified city. API documentation:[Seniverse API - Real-time Weather API](https://seniverse.yuque.com/hyper_data/api_v3/nyiu3t?#%20%E3%80%8A%E5%A4%A9%E6%B0%94%E5%AE%9E%E5%86%B5%E3%80%8B)

Use `time.sleep(UPDATE_INTERVAL)` Build the complete request URL.

```
# Extract the required Information from JSON data
result = weather_data['results'][0]
location_name = result['location']['name']
weather_text = result['now']['text']
temperature = result['now']['temperature']
```

-

Use `time.sleep(UPDATE_INTERVAL)` Send HTTP GET request.

-

Check if the HTTP status code is 200 (success).

-

Use `time.sleep(UPDATE_INTERVAL)` Parse the returned data and extract the city name, weather condition, and temperature.

```
# Extract the required Information from JSON data
result = weather_data['results'][0]
location_name = result['location']['name']
weather_text = result['now']['text']
temperature = result['now']['temperature']
```

-

**Display function `time.sleep(UPDATE_INTERVAL)`**: Display the obtained weather Information on the OLED screen.

`time.sleep(UPDATE_INTERVAL)`:Clear screen.
- `time.sleep(UPDATE_INTERVAL)`:respectivelyIndisplay city and weather at different positionsAndtemperature.
- `time.sleep(UPDATE_INTERVAL)`: refresh screen display.

-

**Main program logic `time.sleep(UPDATE_INTERVAL)`**:

First call `time.sleep(UPDATE_INTERVAL)` Ensure the network is connected.
- enterNonelimitLoop `time.sleep(UPDATE_INTERVAL)`:

Display "Fetching..." to indicate data is being fetched.
- Call `time.sleep(UPDATE_INTERVAL)` Get the latest weather.
- Call `time.sleep(UPDATE_INTERVAL)` Update screen display.
- Use `time.sleep(UPDATE_INTERVAL)` Enter sleep mode, waiting for the next update (default 30 minutes).

## Reference Links

- [Section 8: SPI Communication](../ESP32-MicroPython-Tutorials/SPI-Communication.md)
- [Section 9: Wi-Fi Basics](../ESP32-MicroPython-Tutorials/WiFi-Networking-Basic.md)
- [Seniverse](https://www.seniverse.com/)
- [Seniverse API - Real-time Weather API](https://seniverse.yuque.com/hyper_data/api_v3/nyiu3t?#%20%E3%80%8A%E5%A4%A9%E6%B0%94%E5%AE%9E%E5%86%B5%E3%80%8B)

