/*
  WebGUIApp.ino -- ESP8266 WEB GUI example application for Arduino IDE

  Copyright (c) 2017 Alex Shavlovsky. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

const char* wifi_ssid = ********;
const char* wifi_password = ********;

#include <Arduino.h>
#include <StatusLed.h>
StatusLed WifiLed = StatusLed();
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebSocketsServer.h> //https://github.com/Links2004/arduinoWebSockets
//depends on: https://github.com/me-no-dev/ESPAsyncTCP
//in the file WebSockets.h uncomment the line "#define WEBSOCKETS_NETWORK_TYPE NETWORK_ESP8266"
//to enable the Async TCP mode!
//to reduce a memory consumption in the file WebSocketsServer.h replace
//the line "#define WEBSOCKETS_SERVER_CLIENT_MAX (5)" to "#define WEBSOCKETS_SERVER_CLIENT_MAX  (2)"

WebSocketsServer WsServer = WebSocketsServer(81);
#include <ESP8266WebServer.h>
ESP8266WebServer HttpServer = ESP8266WebServer(80);

#include <ModbusTcpSlave.h>
ModbusTcpSlave MbTcpSlave = ModbusTcpSlave(502, 1); //port = 502, unit id = 1

#include <ESP8266WebSocketsWebGUI.h> // WEB interface and a user side application

#include <MetarProvider.h> // MRTAR weather parser

MetarProvider metar;

#include <SimpleTimer.h> //https://github.com/jfturcot/SimpleTimer
SimpleTimer tasks;

#include <I2cConfig.h>//depends on: https://github.com/pasko-zh/brzo_i2c
#include <I2CEEPROM.h>
#include <I2CLCD.h>
#include <I2cTimeProvider.h>

//======Global Variables====================
bool enWifi = false; // Wifi == WL_CONNECTED
time_t LocalTime = 0; // Local unix time
time_t AbsoluteTime = 0; // Absolute time counts from boot

//============Time management===============
#include <TimeKeeper.h>
#include <NtpTimeProvider.h>
TimeKeeper AbsoluteTimeKeeper;
TimeKeeper ExternalRtcTimeKeeper;
TimeKeeper NtpTimeKeeper;
I2cTimeProvider ExternalRtcTimeProvider;

//======Global Persistent Variables=========
uint16_t *ptrbootCount;
int16_t *ptrInt1;
uint8_t *ptrBool1;
char *ptrString1;

//=======Average Cycle Calc=====
#define AVG_CYCLE_CALC_INTERVAL 1000
uint32_t cyclesCount = 1;
uint32_t avgCycle = 0;
uint32_t curMicros;
uint32_t preMicros = 0;
uint32_t preMaxCycle = 0;
uint32_t curMaxCycle = 0;
void Task_AverageCycleCalc() {
    avgCycle = (uint32_t)1000 * AVG_CYCLE_CALC_INTERVAL / cyclesCount;
    cyclesCount = 1;
    preMaxCycle = curMaxCycle;
    curMaxCycle = 0;
}

void Task_TimeManager() { // call interval = 500 ms
    AbsoluteTime = AbsoluteTimeKeeper.get();
    if(ExternalRtcTimeKeeper.retrySync()) ExternalRtcTimeKeeper.set(ExternalRtcTimeProvider.get());
    if(NtpTimeKeeper.retrySync()) asyncNtpStatus = asyncNtpNeedSync;
    NtpTimeKeeper.set(asyncNtpRun());
    LocalTime=(ExternalRtcTimeKeeper.status >= NtpTimeKeeper.status)?ExternalRtcTimeKeeper.get():NtpTimeKeeper.get();
}

const char TEXT_HTML[] PROGMEM = "text/html";
//==========Websockets response buffer==============
#define ws_bf_size 512
char ws_bf0[ws_bf_size + WEBSOCKETS_MAX_HEADER_SIZE]; // ws send buffer with header size reserved
char *ws_bf = &ws_bf0[WEBSOCKETS_MAX_HEADER_SIZE]; // pure payload ws send buffer


//=============MODBUS RTU slave============
//#include <ModbusRtu.h> //https://github.com/smarmengol/Modbus-Master-Slave-for-Arduino
//Modbus MbRtuSlave(1, 0, 0);
//=============MODBUS slave softwareserial============
//#include <ModbusRtu.h>
//#include <SoftwareSerial.h>
//Modbus MbRtuSlave(1);
//SoftwareSerial mySerial(D3, D4);

String IPToString(IPAddress a) {
    char bf[16];
    snprintf_P(bf, 16, PSTR("%d.%d.%d.%d"), a[0], a[1], a[2], a[3]);
    return String(bf);
}

void getClientIP(char* bf, size_t bfsz, int i);

//=======================Menu structure===========================
MenuItem * Menu1[] = {
    new EEPROMuint16(0, ptrbootCount),
    new MenuFolder(0, 1, "ESP menu"),

    new MenuFolder(1, 10, "System info"),
    new MenuFolder(10, 11, "Board status"),
    new MenuOutputInteger(11, "Chip ID", "", []()->int32_t{return ESP.getChipId();}, 10000),
    new MenuOutputString(11, "IP address", [](char* bf, size_t bfsz)->void{snprintf(bf, bfsz, IPToString(WiFi.localIP()).c_str());}, 10000),
    new MenuOutputInteger(11, "Boot count", "", []()->int32_t{return *ptrbootCount;}, 10000),
    new MenuOutputInteger(11, "Free memory", " b", []()->int32_t{return ESP.getFreeHeap();}, 1000),
    new MenuOutputInteger(11, "Average cycle", " &mu;s", []()->int32_t{return avgCycle;}, AVG_CYCLE_CALC_INTERVAL),
    new MenuOutputInteger(11, "Max cycle", " &mu;s", []()->int32_t{return preMaxCycle;}, AVG_CYCLE_CALC_INTERVAL),
    new MenuOutputInteger(11, "Metar temp", "&deg;C", []()->int32_t{return metar.temp;}, 1000),

    new MenuFolder(10, 12, "System time"),
    new MenuOutputString(12, "System time", [](char* bf, size_t bfsz)->void{getFormatedTime(bf, bfsz, LocalTime);}, 1000),
    new MenuOutputString(12, "Boot time", [](char* bf, size_t bfsz)->void{getFormatedTime(bf, bfsz, LocalTime - AbsoluteTime);}, 10000),
    new MenuOutputString(12, "RTC time", [](char* bf, size_t bfsz)->void{snprintf(bf, bfsz, String(TimeToString(ExternalRtcTimeKeeper.get()) + ' ' + ExternalRtcTimeKeeper.getStatusString()).c_str());}, 1000),
    new MenuOutputString(12, "NTP time", [](char* bf, size_t bfsz)->void{snprintf(bf, bfsz, String(TimeToString(NtpTimeKeeper.get()) + ' ' + NtpTimeKeeper.getStatusString()).c_str());}, 1000),
    new MenuButton(12, "Sync NTP->RTC", []()->void{
        if(NtpTimeKeeper.status == timeSet) {
            ExternalRtcTimeProvider.set(NtpTimeKeeper.get());
            ExternalRtcTimeKeeper.set(ExternalRtcTimeProvider.get());
        }}),

    new MenuFolder(10, 13, "I2C devices"),
    new MenuOutputInteger(13, "EEPROM found", "", []()->int32_t{return enEeprom;}, 1000),
    new MenuOutputInteger(13, "RTC found", "", []()->int32_t{return enRtc;}, 1000),
    new MenuOutputInteger(13, "LCD found", "", []()->int32_t{return enLcd;}, 1000),
    new MenuFolder(10, 14, "Websocket clients"),
    new MenuOutputString(14, "Slot 1", [](char* bf, size_t bfsz)->void{getClientIP(bf, bfsz, 0);}, 5000),
    new MenuOutputString(14, "Slot 2", [](char* bf, size_t bfsz)->void{getClientIP(bf, bfsz, 1);}, 5000),
    new MenuOutputString(14, "Slot 3", [](char* bf, size_t bfsz)->void{getClientIP(bf, bfsz, 2);}, 5000),
    new MenuOutputString(14, "Slot 4", [](char* bf, size_t bfsz)->void{getClientIP(bf, bfsz, 3);}, 5000),
    new MenuOutputString(14, "Slot 5", [](char* bf, size_t bfsz)->void{getClientIP(bf, bfsz, 4);}, 5000),
    new MenuFolder(1, 20, "Configuration"),
    new MenuInputBoolean(20, "Switch example", 1, "OFF", "ON", ptrBool1),
    new MenuInputString(20, "METAR station ID", "UMGG", ptrString1),
    new MenuButton(20, "Clear EEPROM and reset", []()->void{clearEeprom(); ESP.reset();}),
    new MenuFolder(1, 30, "Modbus"),
    new MenuInputInteger(30, "Register 0", 100, "", 0, 1000, ptrInt1),
    new MenuOutputInteger(30, "TCP clients connected", "", []()->int32_t{return MbTcpSlave.status.clients_num;}, 100),
    new MenuOutputInteger(30, "Total requests", "", []()->int32_t{return MbTcpSlave.status.total_requests;}, 100),
    new MenuOutputInteger(30, "Error requests", "", []()->int32_t{return MbTcpSlave.status.total_requests - MbTcpSlave.status.ok_requests;}, 100),
    new MenuOutputInteger(30, "Last error code", "", []()->int32_t{return MbTcpSlave.status.last_error_code;}, 100),
    new MenuOutputInteger(30, "00", "", []()->int32_t{return MbTcpSlave.regs[0];}, 100),
    new MenuOutputInteger(30, "01", "", []()->int32_t{return MbTcpSlave.regs[1];}, 100),
    new MenuOutputInteger(30, "02", "", []()->int32_t{return MbTcpSlave.regs[2];}, 100),
    new MenuOutputInteger(30, "03", "", []()->int32_t{return MbTcpSlave.regs[3];}, 100),
    new MenuOutputInteger(30, "04", "", []()->int32_t{return MbTcpSlave.regs[4];}, 100),
    new MenuOutputInteger(30, "05", "", []()->int32_t{return MbTcpSlave.regs[5];}, 100),
    new MenuOutputInteger(30, "06", "", []()->int32_t{return MbTcpSlave.regs[6];}, 100),
    new MenuOutputInteger(30, "07", "", []()->int32_t{return MbTcpSlave.regs[7];}, 100),
    new MenuOutputInteger(30, "08", "", []()->int32_t{return MbTcpSlave.regs[8];}, 100),
    new MenuOutputInteger(30, "09", "", []()->int32_t{return MbTcpSlave.regs[9];}, 100),
    new MenuOutputInteger(30, "10", "", []()->int32_t{return MbTcpSlave.regs[10];}, 100),
    new MenuOutputInteger(30, "11", "", []()->int32_t{return MbTcpSlave.regs[11];}, 100),
    new MenuOutputInteger(30, "12", "", []()->int32_t{return MbTcpSlave.regs[12];}, 100),
    new MenuOutputInteger(30, "13", "", []()->int32_t{return MbTcpSlave.regs[13];}, 100),
    new MenuOutputInteger(30, "14", "", []()->int32_t{return MbTcpSlave.regs[14];}, 100),
    new MenuOutputInteger(30, "15", "", []()->int32_t{return MbTcpSlave.regs[15];}, 100),
};


void getClientIP(char* bf, size_t bfsz, int i) {
    if(MenuItem::clientLevel[i]) {
        IPAddress tmp = WsServer.remoteIP(i);
        if(tmp != IPAddress()) {
            snprintf(bf, bfsz, IPToString(tmp).c_str());
            return;
        }
        MenuItem::clientLevel[i] = 0;
    }
    snprintf_P(bf, bfsz, PSTR("empty"));
}

void Task_PingClients() {
    for(uint8_t i = 0; i < 5; i++) if(MenuItem::clientLevel[i] && !WsServer.sendPing(i, (uint8_t*)"ESP")) MenuItem::clientLevel[i] = 0;
}

void SendWSBuffer(uint8_t clnt) {// this is the only entry point to send data to WS stream
    WifiLed.setTimeout();
    if(!WsServer.sendTXT(clnt, ws_bf0, strlen(ws_bf), true)) MenuItem::clientLevel[clnt] = 0;
}

void Task_RefreshDisplay() {
    if(!enLcd) return;
    lcd.setCursor(0, 0);
    char tmp[20];
    getFormatedTime(tmp, 20, LocalTime);
    lcd.print(tmp);
    lcd.setCursor(0, 1);
    if(metar.temp != metar.NOT_SET) lcd.print(metar.temp, 1);
    else LCDprintLeft(PSTR("not set"), 10);
    lcd.setCursor(11, 1);
    lcd.printf("%04d", *ptrInt1);
    lcd.setCursor(16, 1);
    lcd.printf("%04d", *ptrbootCount);
    lcd.setCursor(0, 2);
    LCDprintLeft(ptrString1, 10);
    lcd.setCursor(11, 2);
    lcd.printf("%06d", MbTcpSlave.status.total_requests);
    lcd.setCursor(0, 3);
    if(enWifi) LCDprintLeft("", 10);
    else LCDprintLeft(PSTR("no WiFi"), 10);
}

void retainEepromConfig() {
    for(MenuItem* a : Menu1) {
        uint16_t offs;
        uint8_t siz;
        a->getEEPROMPointer(offs, siz);
        for(uint16_t i = 0; i < siz; i++) EepromMirror.data[offs + i] = a->getValToEEPROM(i);
    }
    retainEepromMirror();
}

void submitEepromItemChanges(MenuItem * a) {
    uint16_t offs;
    uint8_t siz;
    a->getEEPROMPointer(offs, siz);
    bool wasChanged = false;
    for(uint16_t i = 0; i < siz; i++) {
        uint8_t tmp = a->getValToEEPROM(i);
        if(tmp != EepromMirror.data[offs + i]) wasChanged = true;
        EepromMirror.data[offs + i] = tmp;
    }
    if(wasChanged) submitEepromMirror(offs, siz);
}

void submitEepromChanges() {
    for(MenuItem* a : Menu1) submitEepromItemChanges(a);
}

void loadEepromConfig() {
    if(loadEepromMirror()) {
        for(MenuItem* a : Menu1) {
            uint16_t offs;
            uint8_t siz;
            a->getEEPROMPointer(offs, siz);
            for(uint16_t i = 0; i < siz; i++) a->setValFromEEPROM(i, EepromMirror.data[offs + i]);
        }
    } else retainEepromConfig();
}

void Task_RefreshI2CDevices() {
    bool LCDoff = (enLcd == 0);
    bool EEPROMoff = (enEeprom == 0);
    getI2cDevices();
    if(EEPROMoff && enEeprom) loadEepromConfig();
    if(LCDoff && enLcd) {
        initLCD();
        Task_RefreshDisplay();
    }
}

void Task_ServeRefreshMessages() {
    for(MenuItem* a : Menu1) {
        a->getRefreshMessage(ws_bf, ws_bf_size);
        if(ws_bf[0]) {
            uint8_t lvl = a->getLevel();
            for(uint8_t i = 0; i < 5; i++) if(MenuItem::clientLevel[i] == lvl) SendWSBuffer(i);
        }
    }
}

void Task_UpdateMetar() {
    if(enWifi) metar.get(ptrString1);
}

void Task_WifiLedBlink() {
    WifiLed.run();
}

void WsDispatcher(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
    case WStype_DISCONNECTED: {
        MenuItem::clientLevel[num] = 0;
        break;
    }
    case WStype_CONNECTED: {
        break;
    }
    case WStype_TEXT: {
        if(payload[0] == 'M') {
            String m_id = (const char *)&payload[1];
            MenuItem::clientLevel[num] = m_id.toInt();
            ws_bf[0] = 'x';
            ws_bf[1] = '\0';
            SendWSBuffer(num);
            for(MenuItem* a : Menu1) {
                a->getHTMLCode(num, ws_bf, ws_bf_size);
                if(ws_bf[0]) SendWSBuffer(num);
                delay(1);
            }
            break;
        }
        if(payload[0] == 'P') {
            String s_val = (const char *)&payload[1];
            String s_id = s_val;
            int i_idx = s_id.indexOf('=');
            s_id.remove(i_idx);
            Menu1[s_id.toInt()]->getResponseMessageToUserInput(s_val.substring(i_idx + 1), ws_bf, ws_bf_size);
            submitEepromItemChanges(Menu1[s_id.toInt()]);
            if(ws_bf[0] != '\0') {
                for(uint8_t clnt = 0; clnt < 5; clnt++) {
                    if(MenuItem::clientLevel[clnt] == MenuItem::clientLevel[num]) SendWSBuffer(clnt);
                }
            }
            break;
        }
    }
    }
}

void setup() {
    delay(100);
    initI2c(); // I2C low level driver init

    enEeprom = pingI2cDevice(EEPROM_I2C_ADDRESS); // check EEPROM is present
    if(enEeprom) {
        loadEepromConfig(); // get config from the EEPROM
        *ptrbootCount = *ptrbootCount + 1; // increase boot counter
        submitEepromChanges(); // and persist it to the EEPROM
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_ssid, wifi_password);

    //===============tasks init===================
    tasks.setInterval(5000, Task_RefreshI2CDevices); // HotPlug for I2C devices
    tasks.setInterval(500, Task_TimeManager); // System time management
    tasks.setInterval(1000, Task_RefreshDisplay); // LCD redraw
    tasks.setInterval(5000, Task_PingClients); // Ping WS clients
    tasks.setInterval(100, Task_ServeRefreshMessages); // Send model changes to WS clients
    tasks.setInterval(30 * 60 * 1000, Task_UpdateMetar); // METAR weather get - TODO make it async
    tasks.setInterval(AVG_CYCLE_CALC_INTERVAL, Task_AverageCycleCalc); // Average loop cycle measurement
    tasks.setInterval(10, Task_WifiLedBlink); // Status led service

    WsServer.onEvent(WsDispatcher); // server side WS dispatcher
    HttpServer.on("/", HTTP_GET, []() {
        HttpServer.send_P(200, TEXT_HTML, HTML_TEMPLATE); //front-end app
    });

    //===============MODBUS RTU slave init===================
    //MbRtuSlave.begin(115200); // MODBUS RTU
    //MbRtuSlave.begin(&mySerial,115200); // Soft serial MODBUS RTU

    WifiLed.setOn(); // Blink status led while WIFI not connected
}

void loop() {
    MbTcpSlave.regs[0] = *ptrInt1;
    MbTcpSlave.regs[1] = millis() % 10000;

    bool offWifi = (enWifi == false);
    enWifi = (WiFi.status() == WL_CONNECTED);
    if(offWifi && enWifi) {
        WsServer.begin();
        HttpServer.begin();
        MbTcpSlave.begin();
        Task_UpdateMetar();
    }
    if(enWifi) {
        HttpServer.handleClient();
        if(MbTcpSlave.run()) WifiLed.setTimeout();
        WifiLed.setOff();
    } else WifiLed.setOn();
    tasks.run();

    yield();

    //MbRtuSlave.poll(MbTcpSlave.regs, REGS_NUM); //MODBUS RTU

    cyclesCount++;
    curMicros = micros();
    if(curMicros - preMicros > curMaxCycle) curMaxCycle = curMicros - preMicros;
    preMicros = curMicros;
}
