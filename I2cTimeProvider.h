/*
  I2cTimeProvider.h -- I2C ZS-042 RTC driver library

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

#include <TimeLib.h> //https://github.com/PaulStoffregen/Time
#include <brzo_i2c.h>

// ZS-042 external RTC+EEPROM (DS1307+AT24C32)
// if you use CR2032 battery do not foget to remove the SMD resistor
// for example look http://www.rigacci.org/wiki/lib/exe/fetch.php/doc/appunti/hardware/raspberrypi/rtc-zs-042-raspberry-pi.jpg
// otherwise your battery will die soon

//==================DS1307 RTC driver====================

class I2cTimeProvider {
    static bool read(tmElements_t &tm);
    static bool write(tmElements_t &tm);
    static uint8_t dec2bcd(uint8_t num);
    static uint8_t bcd2dec(uint8_t num);
public:
    static time_t get();
    static void set(time_t t);
};

time_t I2cTimeProvider::get() { // Aquire data from buffer and convert to time_t
    if(!enRtc) return 0;
    tmElements_t tm;
    if(read(tm) == false) return 0;
    return(makeTime(tm));
}

void I2cTimeProvider::set(time_t t) {
    tmElements_t tm;
    breakTime(t, tm);
    tm.Second |= 0x80;  // stop the clock
    write(tm);
    tm.Second &= 0x7f;  // start the clock
    write(tm);
}

// Aquire data from the RTC chip in BCD format
bool I2cTimeProvider::read(tmElements_t &tm) {
    if(!enRtc) return false;
    uint8_t sec;
    uint8_t bf[7];
    bf[0] = 0;
    brzo_i2c_start_transaction(RTC_I2C_ADDRESS, I2C_CLOCK_RATE);
    brzo_i2c_write(bf, 1, true);
    brzo_i2c_read(bf, tmNbrFields, false);
    if(brzo_i2c_end_transaction()!=0) {
        enRtc=0;
        return false;
    }
    // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
    sec = bf[0];
    tm.Second = bcd2dec(sec & 0x7f);
    tm.Minute = bcd2dec(bf[1]);
    tm.Hour =   bcd2dec(bf[2] & 0x3f);  // mask assumes 24hr clock
    tm.Wday = bcd2dec(bf[3]);
    tm.Day = bcd2dec(bf[4]);
    tm.Month = bcd2dec(bf[5]);
    tm.Year = y2kYearToTm((bcd2dec(bf[6])));
    if(sec & 0x80) return false;  // clock is halted
    return true;
}

bool I2cTimeProvider::write(tmElements_t &tm) {
    if(!enRtc) return false;
    uint8_t bf[8];
    bf[0] = 0; // reset register pointer
    bf[1] = dec2bcd(tm.Second);
    bf[2] = dec2bcd(tm.Minute);
    bf[3] = dec2bcd(tm.Hour);      // sets 24 hour format
    bf[4] = dec2bcd(tm.Wday);
    bf[5] = dec2bcd(tm.Day);
    bf[6] = dec2bcd(tm.Month);
    bf[7] = dec2bcd(tmYearToY2k(tm.Year));
    brzo_i2c_start_transaction(RTC_I2C_ADDRESS, I2C_CLOCK_RATE);
    brzo_i2c_write(bf, 8, false);
    if(brzo_i2c_end_transaction()!=0) {
        enRtc=0;
        return false;
    }
    return true;
}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t I2cTimeProvider::dec2bcd(uint8_t num) {
    return ((num/10 * 16) + (num % 10));
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t I2cTimeProvider::bcd2dec(uint8_t num) {
    return ((num/16 * 10) + (num % 16));
}
