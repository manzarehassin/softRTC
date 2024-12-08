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

Published at 8 Dec, 2024 @ 9:05 PM (UTC+6)

*/


#ifndef soft_rtc_h
#define soft_rtc_h
#include "Arduino.h"

class softRTC
{
  public:
    // Constructor 
   softRTC() {flush();}
    // Methods
    inline void flush() {memset(&clk,0,sizeof(clk));clk.year=24; clk.week=2;clk.day=clk.month=1;}
    inline void begin() {clk.enable_=1;}
    inline void stop()  {clk.enable_=0; flush();}
    
    void write(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, bool ampm, bool mode12h) __attribute__((always_inline));
    void read(uint8_t &day, uint8_t &month, uint16_t &year, uint8_t &hour, uint8_t &minute, uint8_t &second, bool & ampm, bool &mode12h, uint8_t &week) __attribute__((always_inline));
    void print() __attribute__((always_inline));
    void update() __attribute__((always_inline));
    
    inline uint8_t hour()   {return clk.hour;}
    inline uint8_t minute() {return clk.minute;}
    inline uint8_t second() {return clk.second;}
    inline bool is24hrs()   {return !clk.mode12;}
    inline bool isPM()      {return clk.ampm;}
    inline String ampm()    {return clk.ampm ? F("PM") : F("AM");}
    inline uint8_t day()    {return clk.day;}
    inline uint8_t month()  {return clk.month;}
    inline uint16_t year()  {return (20 + clk.century) * 100 + clk.year;}  
    inline String week()    {return getNames(clk.week);}
    String leadingZero(uint8_t value);
    inline bool syncStatus() {return (clk.enable_ and clk.sync_);}

    #define AM 0
    #define PM 1
    #define MODE_12H 1
    #define MODE_24H 0

  private:
  
    inline bool checkLeapYear(uint16_t& year) __attribute__((always_inline));
    inline void manageYear(bool select, uint16_t& Full, uint8_t& Half, bool century) __attribute__((always_inline));
    inline uint8_t getDays(uint16_t& year, uint8_t month) __attribute__((always_inline));
    inline void Convert_To_12h(uint8_t& _Hour, bool& _AMPM) __attribute__((always_inline));
    inline void Convert_To_24h(uint8_t& _Hour, bool & _AMPM) __attribute__((always_inline));
    inline uint8_t getWeekdays(uint8_t day, uint8_t month, uint16_t& year) __attribute__((always_inline));
    inline void hourValidator(uint8_t& hr, bool rw) __attribute__((always_inline)); // rw: 0= read, 1= write
    inline void yearValidator(uint16_t& year, bool rw) __attribute__((always_inline)); // rw: 0= read, 1= write
    inline void errorMsg(uint8_t val) __attribute__((always_inline));
    inline bool syncStatus_() __attribute__((always_inline));
    String getNames(uint8_t val) __attribute__((always_inline));

    struct clockStorage
      {
        uint8_t day    : 5; // 1-31
        uint8_t month  : 4; // 1-12
        uint8_t year   : 7; // 00-99 Short Year
        uint8_t hour   : 5; // 00-23
        uint8_t minute : 6; // 00-59
        uint8_t second : 6; // 00-59
        uint8_t week   : 3; // 1-7
        uint8_t ampm   : 1; // 0= am, 1= pm
        uint8_t mode12 : 1; // 0 = 24hr, 1 = 12h
        uint8_t century: 1; // century bit 0= 2000, 1= 2100
        bool   enable_ : 1; // enable rtc
        bool   sync_   : 1; // Sync Status
      }clk;

    #define rtc_bit_ampm    1
    #define rtc_bit_12h     2
    #define rtc_bit_century 3
    #define rtc_short_year  0
    #define rtc_full_year   1
    #define error_invalid_date  0
    #define error_not_sync      1
};          // end of class
#endif      // end of ifndef soft_rtc_h