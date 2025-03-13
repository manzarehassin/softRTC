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

#include "softRTC.h"
#include <avr/pgmspace.h>
#include <Arduino.h>

// Error message macros
#define ERR_NOT_SYNC() Serial.println(F("Error: Not synced yet."))
#define ERR_INVALID_DATE() Serial.println(F("Error: Invalid date & time."))

uint8_t softRTC::getWeekdays(uint8_t day, uint8_t month, uint16_t year) 
{
  const int y0 = year - (month < 3);
  const int x  = y0 + y0/4 - y0/100 + y0/400;
  const int m0 = month + 12*(month < 3) - 2;
  return (day + x + m0*31/12) % 7 + 1;
}

void softRTC::Convert_To_12h(uint8_t &hour, bool &isPM) 
{
  isPM = (hour >= 12);
  if(hour==0) hour=12; else if(hour>12) hour-=12;
}

void softRTC::Convert_To_24h(uint8_t &hour, bool isPM) 
{
  if(isPM && hour<12) hour+=12; else if(!isPM && hour==12) hour=0;
}

void softRTC::setCenturybit(uint16_t year) 
{
  startclk.century = (year>=2100);
}

void softRTC::manageYear(bool select, uint16_t &Full, uint8_t &Half, bool century) 
{
  if(select)
    Full = (20+century)*100 + Half;
  else
    Half = Full % 100;
}

void softRTC::write(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, bool isPM, bool is12H) 
{
  if(day==0 || day>31 || month==0 || month>12 || minute>59 || second>59 || (is12H==MODE_12H ? (hour==0 || hour>12) : (hour>23))) 
  {
      ERR_INVALID_DATE();
      clkset.sync_ = 0;
      startclk.day = startclk.month = 1;
      return;
  }
  startclk.day   = day;
  startclk.month = month;
  setCenturybit(year);
  uint8_t sy;
  manageYear(rtc_short_year, year, sy, startclk.century);
  startclk.year   = sy;
  startclk.minute = minute;
  startclk.second = second;
  if(is12H)
    Convert_To_24h(hour, isPM);
  startclk.hour = hour;
  clkset.millis = millis();
  clkset.sync_  = 1;
  clkset.is12H  = is12H;
}

void softRTC::read(uint8_t &day, uint8_t &month, uint16_t &year, uint8_t &hour, uint8_t &minute, uint8_t &second, bool &isPM, bool &is12H, uint8_t &week)
{
  if(!syncStatus_())
  {
    day = month = hour = year = minute = second = isPM = is12H = week = 0; return;
  }
  calcTime(year, month, day, hour, minute, second);
  if(clkset.is12H){ Convert_To_12h(hour, isPM); is12H = true; }
  else is12H = false;
  week = getWeekdays(day, month, year);
}

void softRTC::errorMsg(uint8_t val) 
{
  if(val==error_not_sync)
    ERR_NOT_SYNC();
  else if(val==error_invalid_date)
    ERR_INVALID_DATE();
}

bool softRTC::syncStatus_() 
{
  if(!clkset.sync_){ errorMsg(error_not_sync); return false; }
  return true;
}

void softRTC::calcTime(uint16_t &year, uint8_t &month, uint8_t &day,uint8_t &hour, uint8_t &minute, uint8_t &second)
{
  const uint8_t PROGMEM dpm[] = {31,28,31,30,31,30,31,31,30,31,30,31};
  unsigned long secs = (millis()-clkset.millis)/1000UL;
  unsigned long ts = startclk.second+secs;
  second = ts % 60;
  ts = startclk.minute + ts/60;
  minute = ts % 60;
  ts = startclk.hour + ts/60;
  hour = ts % 24;
  unsigned long daysElapsed = ts/24;
  uint8_t sy = startclk.year;
  manageYear(rtc_full_year, year, sy, startclk.century);
  month = startclk.month;
  day = startclk.day;
  while(daysElapsed){
    uint8_t d = pgm_read_byte(&dpm[month-1]);
    if(month==2 && isLeapYear(year)) d = 29;
    uint8_t rem = d - day + 1;
    if(daysElapsed < rem){ day += daysElapsed; break; }
    daysElapsed -= rem;
    day = 1;
    if(++month > 12){ month = 1; year++; }
  }
  if(month==2 && day==29 && !isLeapYear(year)){ day = 1; month = 3; }
}

bool softRTC::isLeapYear(uint16_t year) 
{
  return ((year%4==0)&&(year%100!=0)) || (year%400==0);
}

// The optimized print() writes numbers and weekday names directly without using String.
void softRTC::print(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, bool is12H) 
{
  // Date
  if(day<10) Serial.print('0');
  Serial.print(day); Serial.print('-');
  if(month<10) Serial.print('0');
  Serial.print(month); Serial.print('-');
  Serial.print(year); Serial.print(' ');
  // Time
  if(is12H) 
  {
    bool ampm;
    Convert_To_12h(hour, ampm);
    if(hour<10) Serial.print('0');
    Serial.print(hour); Serial.print(':');
    if(minute<10) Serial.print('0');
    Serial.print(minute); Serial.print(':');
    if(second<10) Serial.print('0');
    Serial.print(second); Serial.print(' ');
    Serial.print(ampm ? "PM" : "AM");
  } 
  else 
  {
    if(hour<10) Serial.print('0');
    Serial.print(hour); Serial.print(':');
    if(minute<10) Serial.print('0');
    Serial.print(minute); Serial.print(':');
    if(second<10) Serial.print('0');
    Serial.print(second); Serial.print(" 24H");
  }
  Serial.print(' ');
  uint8_t wd = getWeekdays(day, month, year);
  switch(wd) {
    case 1: Serial.print(F("Sun")); break;
    case 2: Serial.print(F("Mon")); break;
    case 3: Serial.print(F("Tue")); break;
    case 4: Serial.print(F("Wed")); break;
    case 5: Serial.print(F("Thu")); break;
    case 6: Serial.print(F("Fri")); break;
    case 7: Serial.print(F("Sat")); break;
  }
  Serial.println();
}

void softRTC::print() 
{
  uint8_t m, d, h, min, s;
  uint16_t y;
  calcTime(y, m, d, h, min, s);
  print(y, m, d, h, min, s, clkset.is12H);
}
