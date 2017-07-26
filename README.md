ESP8266WebSocketsWebGUI
=======================

ESP8266 WebSockets Web GUI MVC application example

Description
-----------

This library allows to build a ESP8266 Arduino IDE application, that provides a
real-time user interaction and configuring through a WEB GUI, using a Websocket
connection and a JavaScript front-end app. User settings are persisted in
external I2C EEPROM.

 

Key features
------------

-   a cooperative multitasking based on
    [SimpleTimer](https://github.com/jfturcot/SimpleTimer) library

-   up to 5 simultaneous user connections handling based on
    [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets) and
    [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) libraries

-   400 kHz I2C RTC, EEPROM and LCD drivers based on
    [brzo_i2c](s://github.com/pasko-zh/brzo_i2c) library

-   time management and synchronization (non-blocking NTP and external I2C RTC)

-   modbus TCP non-blocking slave (function (0x03) only), that can handle up to
    5 simultaneous keep-alive connections and can process up to 400 messages per
    second

-   all I2C drivers support HotPlug - you can connect any I2C device without
    resetting the system

-   WiFi HotPlug - user sketch starts immediately after reset regardless of the
    state of the WiFi connection

 

User menu
---------

User menu is designed in a horizontal linear layout style and has a multilevel
hierarchy. The menu consists of items (individual lines). Items are grouped into
levels (folders). А user can interact with menu items only from his current
folder. The supported menu items are as follows:

-   **MenuFolder** - represents a link from current menu level to another

-   **MenuOutputInteger** - displays an integer variable

-   **MenuOutputString** - displays a String variable

-   **MenuInputInteger** - allows to edit and persists to EEPROM an integer
    variable

-   **MenuInputBoolean** - allows to edit and persists to EEPROM a boolean
    variable

-   **MenuInputString** - allows to edit and persists to EEPROM a String
    variable

-   **MenuButton** - represents a button; when clicked, the corresponding
    call-back function is called

 

Principles of operation
-----------------------

-   ESP8266 is listening on port 80/HTTP and 81/WS

-   a new client browser connects on port 80/HTTP and send GET "/" request

-   in response HTTP server sends a HTML page with embedded CSS, SVG and
    JavaScript

-   when the page load is complete, JavaScript function establishes a WebSockets
    connection and send the command "M1" to WS stream

-   having received the command "M1" the WS dispatcher generates the HTML code
    for menu level=1 and sends it to the WS stream

-   on the client side JavaScript dispatcher refreshes page body and displays
    the menu

-   further user interaction occurs within the MVC pattern

 

Example application default config
----------------------------------

**in file WebGUIApp.ino**

const char\* wifi_ssid = \*\*\*\*\*\*\*\*; // WIFI SSID

const char\* wifi_password = \*\*\*\*\*\*\*\*; // WIFI password

**in file StatusLed.h**

\#define STATUS_LED_PIN LED_BUILTIN // Status led pin

**in file NtpTimeProvider.h**

\#define NTP_TIME_ZONE 3 // Time zone

**in file WebGUI.ino**

ModbusTcpSlave MbTcpSlave = ModbusTcpSlave(502, 1); // TCPport = 502, unit ID =
1

**in file ModbusTcpSlave.h**

\#define MAX_CLNT_NUM 2 // Max clients number

\#define CON_KEEP_ALIVE 500 // Session timeout (ms)

\#define REGS_MAX_REQ 16 // Max registers number per request

\#define REGS_NUM 16 // Registers number

**in file I2cConfig.h**

\#define I2C_SDA_PIN D2 // SDA line

\#define I2C_SCL_PIN D1 // SCL line

\#define EEPROM_I2C_ADDRESS 0x57 // EEPROM address

\#define RTC_I2C_ADDRESS 0x68 // RTC address

\#define LCD_I2C_ADDRESS 0x3F // LCD address

\#define I2C_CLOCK_RATE 400 // I2C clock rate (kHz)

\#define I2C_ACK_TIMEOUT 2000 // Default ACK timeout (2 ms)

\#define I2C_ACK_EEPROM_WRITE 10000 // EEPROM write ACK timeout (10 ms)

\#define EEPROM_MIRROR_SIZE 512 // Max EEPROM usage (bytes)
