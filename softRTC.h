/*
softRTC 


MIT License

Copyright (c) 2024 Manzar-E-Hassin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Published at 13 March, 2025 @ 5:22 PM (UTC+6)

*/

#ifndef soft_rtc_h
#define soft_rtc_h
#include "Arduino.h"
#include <stdint.h>

class softRTC 
{
  public:
    // Methods
    void write(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, bool isPM, bool is12H);
    void read(uint8_t &day, uint8_t &month, uint16_t &year, uint8_t &hour, uint8_t &minute, uint8_t &second, bool &isPM, bool &is12H, uint8_t &week);
    void print();
    inline bool syncStatus() { return clkset.sync_; }
    
    // Mode definitions
    #define AM 0
    #define PM 1
    #define MODE_12H 1
    #define MODE_24H 0

  private:
    bool isLeapYear(uint16_t year);
    void manageYear(bool select, uint16_t &Full, uint8_t &Half, bool century);
    void Convert_To_12h(uint8_t &hour, bool &isPM);
    void Convert_To_24h(uint8_t &hour, bool isPM);
    uint8_t getWeekdays(uint8_t day, uint8_t month, uint16_t year);
    void errorMsg(uint8_t val);
    bool syncStatus_();
    void setCenturybit(uint16_t year);
    void calcTime(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second);
    void print(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool is12H);

    // Starting date/time settings (packed to save RAM)
    struct __attribute__((packed)) rtcSettings {
      bool sync_ : 1;      // Sync status
      bool is12H : 1;      // 12-hour mode flag
      uint32_t millis;     // Stored millis() value when set
    } clkset;

    #define rtc_short_year  0
    #define rtc_full_year   1
    #define error_invalid_date  0
    #define error_not_sync      1

  protected:
    // Packed time values (minimizes RAM usage)
    struct __attribute__((packed)) clocksyncTime {
      uint8_t day    : 5; // 1-31
      uint8_t month  : 4; // 1-12
      uint8_t year   : 7; // 00-99 (short year)
      uint8_t hour   : 5; // 00-23
      uint8_t minute : 6; // 00-59
      uint8_t second : 6; // 00-59
      uint8_t century: 1; // 0 = 2000, 1 = 2100
    } startclk;
};
#endif // soft_rtc_h
