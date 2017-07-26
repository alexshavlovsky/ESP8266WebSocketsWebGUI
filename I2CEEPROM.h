/*
  I2CEEPROM.h -- I2C ZS-042 EEPROM driver library

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

#include "brzo_i2c.h" // https://github.com/pasko-zh/brzo_i2c

// ZS-042 external RTC+EEPROM (DS1307+AT24C32)
// if you use CR2032 battery do not foget to remove the SMD resistor
// for example look http://www.rigacci.org/wiki/lib/exe/fetch.php/doc/appunti/hardware/raspberrypi/rtc-zs-042-raspberry-pi.jpg
// otherwise your battery will die soon

//==================AT24C32 EEPROM driver====================

//=================mid lewel EEPROM driver===================

uint8_t getEepromAddress(const uint16_t address) { // read 1 byte from the EEPROM address
    if (!enEeprom) return 0;
    uint8_t bf[2];
    bf[0]=(uint8_t)(address >> 8);
    bf[1]=(uint8_t)(address & 0xFF);
    brzo_i2c_start_transaction(EEPROM_I2C_ADDRESS, I2C_CLOCK_RATE);
    brzo_i2c_write(bf, 2, true);
    brzo_i2c_read(bf, 1, false);
    if (brzo_i2c_end_transaction()!=0) {enEeprom=0; return 0;}
    return bf[0];
}

bool setEepromAddress(const uint16_t address, const uint8_t data) { // write 1 byte to the EEPROM address
    if (!enEeprom) return 0;
    uint8_t bf[3];
    bf[0]=(uint8_t)(address >> 8);
    bf[1]=(uint8_t)(address & 0xFF);
    bf[2]=data;
    brzo_i2c_start_transaction(EEPROM_I2C_ADDRESS, I2C_CLOCK_RATE);
    brzo_i2c_write(bf, 3, false);
    brzo_i2c_ACK_polling(I2C_ACK_EEPROM_WRITE);
    if (brzo_i2c_end_transaction()!=0) {enEeprom=0; return false;}
    return true;
}

void setEepromAddressVerify(const uint16_t address, const uint8_t data) { // write 1 byte to the EEPROM address with verify
    uint8_t attempt=3;
    do {
	attempt--;
        setEepromAddress(address,data);
    } while((getEepromAddress(address)!=data)&&(attempt>0));
}

//==========the local mirror of the EEPROM structure============
struct {
    uint32_t crc32;
    uint8_t data[EEPROM_MIRROR_SIZE];
} EepromMirror;

uint32_t calculateCrc32(const uint8_t *data, size_t length) {
    uint32_t crc = 0xffffffff;
    while(length--) {
        uint8_t c = *data++;
        for(uint32_t i = 0x80; i > 0; i >>= 1) {
            bool bit = crc & 0x80000000;
            if(c & i) bit = !bit;
            crc <<= 1;
            if(bit) crc ^= 0x04c11db7;
        }
    }
    return crc;
}

//=================high level user functions===================

bool checkEepromCrc() {
    return (EepromMirror.crc32 == calculateCrc32(((uint8_t*) &EepromMirror) + 4, EEPROM_MIRROR_SIZE));
}

void updateEepromCrc() {
    EepromMirror.crc32 = calculateCrc32(((uint8_t*) &EepromMirror) + 4, EEPROM_MIRROR_SIZE);
}

bool loadEepromMirror() {
    for(uint16_t i=0; i<4+EEPROM_MIRROR_SIZE; i++) *((uint8_t*)&EepromMirror+i)=getEepromAddress(i);
    return checkEepromCrc();
}

void retainEepromMirror() {
    updateEepromCrc();
    for(uint16_t i=0; i<4+EEPROM_MIRROR_SIZE; i++) setEepromAddress(i,*((uint8_t*)&EepromMirror+i));
}

void clearEeprom() {
    for(uint16_t i=0; i<4+EEPROM_MIRROR_SIZE; i++) setEepromAddress(i,255);
}

void submitEepromMirror(const uint16_t offset, const uint16_t size) {
    updateEepromCrc();
    for(uint16_t i=0; i<4; i++) setEepromAddress(i,*((uint8_t*)&EepromMirror+i));
    for(uint16_t i=offset+4; i<offset+4+size; i++) setEepromAddress(i,*((uint8_t*)&EepromMirror+i));
}
