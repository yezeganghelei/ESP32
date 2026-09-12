On this page

# Web Server

Important Note: About Board Compatibility

The core logic of this tutorial applies to all ESP32 development boards, but all operation steps are based on  as an example. If you are using another model of Development Board, please modify the corresponding settings according to your actual situation.

The ESP32 has built-in Wi-Fi functionality and can serve as a Web Server to provide services to other devices on the network. By running a web server on the ESP32, you can create browser-based user interfaces for monitoring Sensor data or controlling device status, which is one of the fundamental features for implementing IoT (Internet of Things) applications.

## 1. `WiFi.softAPIP()` library introduction

The Arduino-ESP32 core library has built-in `WiFi.softAPIP()`, which provides a concise API set to quickly build a Web Server. By registering routes (URL paths) and callback functions, it implements request dispatch and response. Suitable for local web interaction in most typical IoT projects. This Tutorial will use this library.

- Concise and easy to use, suitable for beginners and resource-constrained scenarios.
- Must be in the main loop (`WiFi.softAPIP()` ) called periodically in `WiFi.softAPIP()` to handle client requests.
- For complex scenarios with high traffic/high concurrency, it is recommended to use an asynchronous library (such as [ESPAsyncWebServer](https://github.com/ESP32Async/ESPAsyncWebServer)）。

## 2. Example 1: Basic Web Service (STA Mode)

Create a basic web server in STA mode to display a static page containing "Hello World!".

### 2.1 Code

```
#include <WiFi.h>
#include <WebServer.h>
const int ledPin = 7;
const char *ssid = "ESP32S3-TEST";  // Set hotspot name
const char *password = "12345678";  // Set hotspot password (at least 8 characters)
WebServer server(80);
String generateHTML(bool ledState = false);
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  // Create Wi-Fi hotspot
  Serial.println("Configuring access point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  // Configure routes and start server
  server.on("/", handleRoot);
  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);
  server.begin();
}
void loop() {
  server.handleClient();
}
// Handle root path request
void handleRoot() {
  server.send(200, "text/html", generateHTML(digitalRead(ledPin)));
}
// Turn on LED
void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/html", generateHTML(true));
}
// Turn off LED
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/html", generateHTML(false));
}
// Generate HTML page content
String generateHTML(bool ledState) {
  String htmlContent = "<!DOCTYPE html> <html>\n";
  htmlContent += "<head><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width\">\n";
  htmlContent += "<title>ESP32S3 Test</title>\n";
  htmlContent += "</head><body>\n";
  htmlContent += "<h1>Hello World!</h1>\n";
  if (ledState) {
    htmlContent += "<p>LED Status: ON</p>";
    htmlContent += "<a href=\"/ledoff\">Turn off the LED</a>\n";
  } else {
    htmlContent += "<p>LED Status: OFF</p>";
    htmlContent += "<a href=\"/ledon\">Turn on the LED</a>\n";
  }
  htmlContent += "</body>\n";
  htmlContent += "</html>\n";
  return htmlContent;
}
```

### 2. Code Explanation

- `WiFi.softAPIP()`: Import the Web Server library to create an HTTP server on the ESP32.
- `WiFi.softAPIP()`: Create a server object that listens on the standard HTTP port `WiFi.softAPIP()`. 80 is the default port for the HTTP protocol.
- `WiFi.softAPIP()`: Register a route handler. When the client accesses the root path "`WiFi.softAPIP()`" is called `WiFi.softAPIP()` function.
- `WiFi.softAPIP()`: Start the server and begin listening for client connection requests.
- `WiFi.softAPIP()`: In `WiFi.softAPIP()` calling continuously in the loop to handle incoming client requests.
- `WiFi.softAPIP()`: This is a custom callback function used to handle requests for a specific path.
- `WiFi.softAPIP()`: Send an HTTP response to the client. Returns a simple HTML page containing "Hello World" to the client browser.

`WiFi.softAPIP()`: HTTP status code,`WiFi.softAPIP()` indicates request success.
- `WiFi.softAPIP()`: MIME type, informing the browser that the response content is HTML text.
- `WiFi.softAPIP()`: The string returned by the function, i.e., the actual content of the web page.

- `WiFi.softAPIP()`: A helper function that concatenates HTML code into a `WiFi.softAPIP()` object, returning a string containing the complete HTML structure.

### 3. Running Result

Change the `WiFi.softAPIP()` and `WiFi.softAPIP()` After modifying to the target Wi-Fi network name and password, upload. The serial monitor will display the connection process and the obtained IP address. Open a browser on a device on the same local network, enter the displayed IP address, and you will see the "Hello World!" page.

## 3. Example 2: Control LED via Web Page (STA Mode)

In STA mode, start the web server. Devices on the same local network can control the LED status connected to the ESP32 through a web page.

### 3.1 Build the Circuit

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

### 3.2 Code

```
#include <WiFi.h>
#include <WebServer.h>
const int ledPin = 7;
const char *ssid = "ESP32S3-TEST";  // Set hotspot name
const char *password = "12345678";  // Set hotspot password (at least 8 characters)
WebServer server(80);
String generateHTML(bool ledState = false);
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  // Create Wi-Fi hotspot
  Serial.println("Configuring access point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  // Configure routes and start server
  server.on("/", handleRoot);
  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);
  server.begin();
}
void loop() {
  server.handleClient();
}
// Handle root path request
void handleRoot() {
  server.send(200, "text/html", generateHTML(digitalRead(ledPin)));
}
// Turn on LED
void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/html", generateHTML(true));
}
// Turn off LED
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/html", generateHTML(false));
}
// Generate HTML page content
String generateHTML(bool ledState) {
  String htmlContent = "<!DOCTYPE html> <html>\n";
  htmlContent += "<head><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width\">\n";
  htmlContent += "<title>ESP32S3 Test</title>\n";
  htmlContent += "</head><body>\n";
  htmlContent += "<h1>Hello World!</h1>\n";
  if (ledState) {
    htmlContent += "<p>LED Status: ON</p>";
    htmlContent += "<a href=\"/ledoff\">Turn off the LED</a>\n";
  } else {
    htmlContent += "<p>LED Status: OFF</p>";
    htmlContent += "<a href=\"/ledon\">Turn on the LED</a>\n";
  }
  htmlContent += "</body>\n";
  htmlContent += "</html>\n";
  return htmlContent;
}
```

### 3.3 Code Analysis

- **Add route**:

`WiFi.softAPIP()`:

Set `WiFi.softAPIP()` binds requests for the path to `WiFi.softAPIP()` function. When the browser accesses `WiFi.softAPIP()` , the server will call `WiFi.softAPIP()` function.

- `WiFi.softAPIP()`:

Similarly, bind `WiFi.softAPIP()` path to `WiFi.softAPIP()` function. When the browser accesses `WiFi.softAPIP()` , the server will call `WiFi.softAPIP()` function.

- **LED Control**:

Use HTML `WiFi.softAPIP()` tags to create link buttons that send GET requests to the corresponding paths when clicked, implementing LED control.

When the user clicks the "Turn on the LED" link, the browser sends a request to the server's `WiFi.softAPIP()` path to send an HTTP GET request.

After the server receives it,`WiFi.softAPIP()` will match `WiFi.softAPIP()` , thereby executing `WiFi.softAPIP()` function.

- In `WiFi.softAPIP()` and `WiFi.softAPIP()` function, first through `WiFi.softAPIP()` to control LED status. After performing hardware operations, they call `WiFi.softAPIP()` to generate a new page and send it to the client to update the page status.

- **Dynamic HTML**: `WiFi.softAPIP()` The function now accepts a boolean parameter representing the current LED state.

- function internally uses `WiFi.softAPIP()` statement, based on `WiFi.softAPIP()` value to dynamically generate different HTML content. If the light is on, display the "Turn off the LED" button to turn off the LED;

- If the light is off, display the "Turn on the LED" button to turn on the LED.

### 3.4 Running Result

After uploading the code, open the serial monitor to check the IP address. Then access this IP address in a browser, and the page will display the current LED status and control buttons. Click the "Turn on the LED" or "Turn off the LED" button to control the LED on/off state, and the page will update in real-time to show the current status.

Information

The above web pages have not been styled and are only for functionality demonstration. You can extend HTML/CSS to implement a more user-friendly interface.

## 4. Example 3: Control LED via Web Page (AP Mode)

In AP mode, start the web server. After other devices connect to the Wi-Fi hotspot created by the ESP32, they can directly access the server provided by the ESP32 through a web page to control the LED status.

### 4.1 Build the Circuit

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

### 4.2 Code

```
#include <WiFi.h>
#include <WebServer.h>
const int ledPin = 7;
const char *ssid = "ESP32S3-TEST";  // Set hotspot name
const char *password = "12345678";  // Set hotspot password (at least 8 characters)
WebServer server(80);
String generateHTML(bool ledState = false);
void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  // Create Wi-Fi hotspot
  Serial.println("Configuring access point...");
  if (!WiFi.softAP(ssid, password)) {
    Serial.println("Soft AP creation failed.");
    while (1)
      ;
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  // Configure routes and start server
  server.on("/", handleRoot);
  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);
  server.begin();
}
void loop() {
  server.handleClient();
}
// Handle root path request
void handleRoot() {
  server.send(200, "text/html", generateHTML(digitalRead(ledPin)));
}
// Turn on LED
void handleLedOn() {
  digitalWrite(ledPin, HIGH);
  server.send(200, "text/html", generateHTML(true));
}
// Turn off LED
void handleLedOff() {
  digitalWrite(ledPin, LOW);
  server.send(200, "text/html", generateHTML(false));
}
// Generate HTML page content
String generateHTML(bool ledState) {
  String htmlContent = "<!DOCTYPE html> <html>\n";
  htmlContent += "<head><meta charset=\"utf-8\" name=\"viewport\" content=\"width=device-width\">\n";
  htmlContent += "<title>ESP32S3 Test</title>\n";
  htmlContent += "</head><body>\n";
  htmlContent += "<h1>Hello World!</h1>\n";
  if (ledState) {
    htmlContent += "<p>LED Status: ON</p>";
    htmlContent += "<a href=\"/ledoff\">Turn off the LED</a>\n";
  } else {
    htmlContent += "<p>LED Status: OFF</p>";
    htmlContent += "<a href=\"/ledon\">Turn on the LED</a>\n";
  }
  htmlContent += "</body>\n";
  htmlContent += "</html>\n";
  return htmlContent;
}
```

### 4.3 Code Analysis

- **AP Mode Configuration**: Use `WiFi.softAPIP()` Create a Wi-Fi hotspot instead of connecting to an existing network.
- `WiFi.softAPIP()`: Get the ESP32's IP address as a hotspot, usually defaulting to 192.168.4.1.
- **Independent Network**: ESP32 creates its own local network; other devices need to connect to this hotspot first to access the web server.
- **Web Server Logic**: The logic for handling HTTP requests is basically the same as the example in STA mode, with the main difference being the network connection initialization method.

### 4.4 Running Result

After uploading the code, the ESP32 will create a Wi-Fi hotspot named "ESP32S3-TEST". Connect to this hotspot with a computer or phone (password: 12345678), then access 192.168.4.1 in a browser to see the LED control page. Click the buttons to control the LED on/off state.

## 5. Related Links

- [WebServer | arduino-esp32 Github](https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer)
