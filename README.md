ESP8266WebSocketsWebGUI
=======================

ESP8266 WebSockets Web GUI MVC application example

10.09.2017 - Realtime charts with SmoothieCharts.js upgrade

- SmoothieCharts.js library included (SmoothieJS.h)
- Moving average filter included (FilterAVG.h)
- BME280 sensor library forked (https://github.com/finitespace/BME280)
  and now it is compatible with brzo_i2c
  (install https://github.com/alexshavlovsky/BME280.git)
- ESP8266WebSocketsWebGUI.h upgraded to v1.1
- MenuOutputChart MenuItem added allows to draw realtime charts
  using Smoothie Charts JS library (smoothiecharts.org)
- Migrate to ESP8266 core v.2.4.0-rc1 to enable proper
  floating point formatting whith sprintf function
- Example app included (WebGUIAppBME.ino) that demonstrates
  real-time charts using measurements from the BME280 sensor

![](https://github.com/alexshavlovsky/ESP8266WebSocketsWebGUI/blob/master/Screenshots/9_bme280.png)


Description
-----------

This library allows to build a ESP8266 Arduino IDE application, that provides a
real-time user interaction and configuring through a WEB GUI, using a Websocket
connection and a JavaScript front-end app. User settings are auto mapped and
persisted in external I2C EEPROM.

 

Key features
------------

-   a cooperative multitasking based on
    [SimpleTimer](https://github.com/jfturcot/SimpleTimer) library

-   up to 5 simultaneous user connections handling based on
    [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets) and
    [ESPAsyncTCP](https://github.com/me-no-dev/ESPAsyncTCP) libraries

-   400 kHz I2C RTC, EEPROM and LCD drivers based on
    [brzo_i2c](https://github.com/pasko-zh/brzo_i2c) library

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

![](https://github.com/alexshavlovsky/ESP8266WebSocketsWebGUI/blob/master/Screenshots/3_board_status.png)

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

 

Hardware configuration
----------------------

The minimal hardware configuration:

-   the only ESP8266 board will always run on default settings and will get a
    system time by NTP protocol

The maximal hardware configuration:

-   ESP8266 NodeMcu development board

-   [ZS-042 external RTC+EEPROM (DS1307+AT24C32)
    board](https://ru.aliexpress.com/item/DS3231-AT24C32-IIC-High-Precision-RTC-Module-Clock-Timer-Memory-Module/32666603579.html)

-   [I2C 2004 20X4 character LCD Display
    board](https://ru.aliexpress.com/item/IIC-I2C-TWI-SP-Interface2004-20x4/32675169557.html)

 

Installation
------------

-   install the library https://github.com/Links2004/arduinoWebSockets

in the file WebSockets.h comment the line

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define WEBSOCKETS_NETWORK_TYPE NETWORK_ESP8266
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

and uncomment the line

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define WEBSOCKETS_NETWORK_TYPE NETWORK_ESP8266_ASYNC
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

in the file WebSocketsServer.h

replace the line

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define WEBSOCKETS_SERVER_CLIENT_MAX (5)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

to

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define WEBSOCKETS_SERVER_CLIENT_MAX (2)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

-   install the library https://github.com/me-no-dev/ESPAsyncTCP

-   install the library https://github.com/jfturcot/SimpleTimer

-   install the library https://github.com/pasko-zh/brzo_i2c

-   install the library https://github.com/PaulStoffregen/Time

 

Example application default config
----------------------------------

**in the file WebGUIApp.ino**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const char* wifi_ssid = ********; // WIFI SSID
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const char* wifi_password = ********; // WIFI password
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**in the file StatusLed.h**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define STATUS_LED_PIN LED_BUILTIN // Status led pin
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**in the file NtpTimeProvider.h**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define NTP_TIME_ZONE 3 // Time zone
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**in the file WebGUI.ino**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ModbusTcpSlave MbTcpSlave = ModbusTcpSlave(502, 1); // TCPport = 502, unit ID = 1
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**in the file ModbusTcpSlave.h**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define MAX_CLNT_NUM 2 // Max clients number
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define CON_KEEP_ALIVE 500 // Session timeout (ms)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define REGS_MAX_REQ 16 // Max registers number per request
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define REGS_NUM 16 // Registers number
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**in the file I2cConfig.h**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_SDA_PIN D2 // SDA line
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_SCL_PIN D1 // SCL line
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define EEPROM_I2C_ADDRESS 0x57 // EEPROM address
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define RTC_I2C_ADDRESS 0x68 // RTC address
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define LCD_I2C_ADDRESS 0x3F // LCD address
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_CLOCK_RATE 400 // I2C clock rate (kHz)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_ACK_TIMEOUT 2000 // Default ACK timeout (2 ms)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define I2C_ACK_EEPROM_WRITE 10000 // EEPROM write ACK timeout (10 ms)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define EEPROM_MIRROR_SIZE 512 // Max EEPROM usage (bytes)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
