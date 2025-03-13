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
    // Constructor 
   //softRTC() {}
    // Methods
    void write(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, bool isPM, bool is12H);
    void read(uint8_t &day, uint8_t &month, uint16_t &year, uint8_t &hour, uint8_t &minute, uint8_t &second, bool & isPM, bool &is12H, uint8_t &week);
    void print();
    String Weekdays(uint8_t week)  __attribute__((always_inline));
    inline bool syncStatus() {return (clkset.sync_);}
    String leadingZero(uint8_t value) __attribute__((always_inline));
    
    #define AM 0
    #define PM 1
    #define MODE_12H 1
    #define MODE_24H 0

  private:

    bool isLeapYear(uint16_t year) __attribute__((always_inline));
    inline void manageYear(bool select, uint16_t& Full, uint8_t& Half, bool century);
    inline void Convert_To_12h(uint8_t &hour, bool &isPM) __attribute__((always_inline));
    inline void Convert_To_24h(uint8_t &hour, bool isPM) __attribute__((always_inline));
    inline uint8_t getWeekdays(uint8_t day, uint8_t month, uint16_t year) __attribute__((always_inline));
    inline void errorMsg(uint8_t val) __attribute__((always_inline));
    inline bool syncStatus_() __attribute__((always_inline));
    void setCenturybit(uint16_t year) __attribute__((always_inline));
    void calcTime(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second);
    void print(uint16_t year, uint8_t month, uint8_t day,uint8_t hour, uint8_t minute, uint8_t second, bool is12H);

    // Starting date and time
    struct  __attribute__((packed)) rtcSettings
    {
      bool   sync_   : 1; // Sync Status
      bool    is12H  : 1; // 12h time
      uint32_t millis; // Store the millis() value when the simulation starts
    } clkset;

    #define rtc_bit_ampm    1
    #define rtc_bit_12h     2
    #define rtc_bit_century 3
    #define rtc_short_year  0
    #define rtc_full_year   1
    #define error_invalid_date  0
    #define error_not_sync      1

  protected:

    struct  __attribute__((packed)) clocksyncTime
      {
        uint8_t day    : 5; // 1-31
        uint8_t month  : 4; // 1-12
        uint8_t year   : 7; // 00-99 Short Year
        uint8_t hour   : 5; // 00-23
        uint8_t minute : 6; // 00-59
        uint8_t second : 6; // 00-59
        uint8_t century: 1; // century bit 0= 2000, 1= 2100
      } startclk;

};          // end of class
#endif      // end of ifndef soft_rtc_h
