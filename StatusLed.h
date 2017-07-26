/*
  StatusLed.h -- simple status LED service

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
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

//======Status LED service==============

#define STATUS_LED_PIN LED_BUILTIN
#define STATUS_PIN_NOT_SET 255

class StatusLed {
    uint8_t pinState=STATUS_PIN_NOT_SET;
    void ledOn() {
        if(pinState!=0) {
            pinState=0;
            digitalWrite(STATUS_LED_PIN, LOW);
        }
    }
    void ledOff() {
        if(pinState!=1) {
            pinState=1;
            digitalWrite(STATUS_LED_PIN, HIGH);
        }
    }
public:
    uint8_t mode = 0;
    uint32_t interval = 500;
    uint8_t onoffratio = 25;
    uint32_t timeout = 20;
    uint32_t start;

    void run() {
        if(pinState==STATUS_PIN_NOT_SET) {
            pinMode(LED_BUILTIN, OUTPUT);
            ledOff();
        }
        switch(mode) {
        case 0:
            ledOff();
            break;
        case 1:
            if((!interval)||((millis()-start)%interval<interval*onoffratio/100)) ledOn(); else ledOff();
            break;
        case 2:
            if(millis()-start<timeout) ledOn();
            else {
                mode=0;
                ledOff();
            }
            break;
        }

    }

    void setOn() {
        if(mode==0) {
            mode=1;
            start=millis();
            run();
        }
    }

    void setTimeout() {
        if(mode==0) {
            mode=2;
            start=millis();
            run();
        }
    }

    void setOff() {
        if(mode==1) {
            mode=0;
            run();
        }
    }
};
