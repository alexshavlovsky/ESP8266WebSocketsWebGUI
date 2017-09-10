/*
  I2cConfig.h -- I2C config library

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

#define I2C_SDA_PIN D2
#define I2C_SCL_PIN D1
#define EEPROM_I2C_ADDRESS 0x57
#define RTC_I2C_ADDRESS 0x68
#define LCD_I2C_ADDRESS 0x3F
#define BME_I2C_ADDRESS 0x76
#define I2C_CLOCK_RATE 400
#define I2C_ACK_TIMEOUT 2000
#define I2C_ACK_EEPROM_WRITE 10000

#define EEPROM_MIRROR_SIZE 512 // max size 4096

bool enLcd=false;
bool enRtc=false;
bool enEeprom=false;
bool enBme=false;

bool pingI2cDevice(uint8_t devAddr) { // check I2C device alive
    brzo_i2c_start_transaction(devAddr, I2C_CLOCK_RATE);
    brzo_i2c_ACK_polling(I2C_ACK_TIMEOUT);
    return(brzo_i2c_end_transaction()==0);
}

void getI2cDevices(){ // ping all supported devices
  enLcd=pingI2cDevice(LCD_I2C_ADDRESS);
  enRtc=pingI2cDevice(RTC_I2C_ADDRESS);
  enEeprom=pingI2cDevice(EEPROM_I2C_ADDRESS);
  enBme=pingI2cDevice(BME_I2C_ADDRESS);
}

void initI2c(){
  brzo_i2c_setup(I2C_SDA_PIN,I2C_SCL_PIN,I2C_ACK_TIMEOUT);
}
