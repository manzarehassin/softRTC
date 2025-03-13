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

uint8_t softRTC::getWeekdays(uint8_t day, uint8_t month, uint16_t year) 
{
  const int y0 = year - (month < 3);
  const int x = y0 + y0/4 - y0/100 + y0/400;
  const int m0 = month + 12 * (month < 3) - 2;
  return (day + x + m0*31/12) % 7 + 1;
}

void softRTC::Convert_To_12h(uint8_t &hour, bool &isPM) 
{
  isPM = (hour >= 12);
  if (hour == 0) { hour = 12; }
  else if (hour > 12) { hour -= 12; } 
}

void softRTC::Convert_To_24h(uint8_t &hour, bool isPM)
{
  if (isPM && hour < 12) { hour += 12;} 
  else if (!isPM && hour == 12) { hour = 0;}
}

void softRTC::setCenturybit(uint16_t year) 
{
  startclk.century = (year >= 2100);
}

void softRTC::manageYear(bool select, uint16_t& Full, uint8_t& Half, bool century)
{
  if (select) { Full = (20 + century) * 100 + Half; } // rtc_full_year
  else { Half = Full % 100; } // rtc_short_year
}

void softRTC::write(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, bool isPM, bool is12H) 
{
  // Validate date/time values.
  // Note: day and month are checked for zero as 0 is invalid.
  // For 12-hour mode (MODE_12H), valid hours are 1–12.
  // For 24-hour mode (MODE_24H), valid hours are 0–23.
  if (day == 0 || day > 31 || month == 0 || month > 12 || minute > 59 || second > 59 || (is12H == MODE_12H ? (hour == 0 || hour > 12) : (hour > 23)))
  {
    errorMsg(error_invalid_date);
    //memset(&startclk,0,sizeof(startclk));
    clkset.sync_=0;
    startclk.month=startclk.day=1;
    return;
  }

  startclk.day = day;
  startclk.month = month;
  setCenturybit(year);

  uint8_t buffer;
  manageYear(rtc_short_year, year, buffer, startclk.century);
  startclk.year = buffer;
  startclk.minute = minute;
  startclk.second = second;
  
  // Convert hour to 24-hour format if necessary.
  if (is12H)
  {
    Convert_To_24h(hour, isPM);
  }
  startclk.hour = hour;
  clkset.millis = millis();
  clkset.sync_ = 1;
  clkset.is12H=is12H;
}

void softRTC::read(uint8_t &day, uint8_t &month, uint16_t &year, uint8_t &hour, uint8_t &minute, uint8_t &second, bool &isPM, bool &is12H, uint8_t &week)
{
  if (!syncStatus_()) { day = month = hour = year = minute = second = isPM = is12H = week = 0; return; }
  calcTime(year, month, day, hour, minute, second);
  if (is12H) { Convert_To_12h(hour, isPM); }
  week = getWeekdays(day, month, year);
}

String softRTC::Weekdays(uint8_t week)
{
    switch(week)
    {
      case 1: return F("Sun"); break;
      case 2: return F("Mon"); break;
      case 3: return F("Tue"); break;
      case 4: return F("Wed"); break;
      case 5: return F("Thu"); break;
      case 6: return F("Fri"); break;
      case 7: return F("Sat"); break;
    };
}

String softRTC::leadingZero(uint8_t value)
{
  if((value<10) or (!value)) { return String(F("0")) + String(value); }
  else { return String(value); }
}

bool softRTC::syncStatus_() 
{
  if (!clkset.sync_) { errorMsg(error_not_sync); return 0;}
  return 1;
}

void softRTC::errorMsg(uint8_t val)
{
  if(val == error_not_sync) { Serial.println(F("Error: Not synced yet."));}
  else if(val == error_invalid_date) {Serial.println(F("Error: Invalid date & time."));}
}

void softRTC::calcTime(uint16_t &year, uint8_t &month, uint8_t &day, uint8_t &hour, uint8_t &minute, uint8_t &second) 
{
  const uint8_t PROGMEM daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  const unsigned long elapsedMillis = millis() - clkset.millis; 
  const unsigned long elapsedSeconds = elapsedMillis / 1000; // Convert to total elapsed seconds

  // Calculate the seconds, minutes, and hours
  second = (startclk.second + elapsedSeconds) % 60;
  const unsigned long totalMinutes = startclk.minute + (startclk.second + elapsedSeconds) / 60;
  minute = totalMinutes % 60;
  const unsigned long totalHours = startclk.hour + totalMinutes / 60;
  hour = totalHours % 24;

  // Calculate the day, month, and year
  unsigned long totalDays = totalHours / 24;

  month= startclk.year; // temporary storage for short year
  manageYear(rtc_full_year, year, month, startclk.century); // convert to full year
  month = startclk.month;
  day = startclk.day;

  while (totalDays > 0) 
  {
    uint8_t daysInCurrentMonth = pgm_read_byte(&daysInMonth[month - 1]);

    // Adjust for leap year if February
    if (month == 2 && isLeapYear(year)) 
    {
      daysInCurrentMonth = 29;
    }

    if (totalDays < daysInCurrentMonth - day + 1) 
    {
      day += totalDays;
      totalDays = 0;
    } 
    else 
    {
      totalDays -= (daysInCurrentMonth - day + 1);
      day = 1;
      month++;
      
      if (month > 12) 
      {
        month = 1;
        year++;
      }
    }
  }
  // ✅ FIX: If the date lands on February 29 in a **non-leap year**, move to March 1
  if (month == 2 && day == 29 && !isLeapYear(year)) { day = 1; month = 3; }
  
}

bool softRTC::isLeapYear(uint16_t year) 
{ 
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void softRTC::print(uint16_t year, uint8_t month, uint8_t day,uint8_t hour, uint8_t minute, uint8_t second, bool is12H)
{
  Serial.print(day);
  Serial.print(F("-"));
  Serial.print(month);
  Serial.print(F("-"));
  Serial.print(year);
  Serial.print(F(" "));

  day = is12H; // temporary storage
  //bool ampm;
  if(day) // Checking for 12h
  {
    Convert_To_12h(hour, is12H); //is12H holds the ampm value
  }
  Serial.print(leadingZero(hour));
  Serial.print(F(":"));
  Serial.print(leadingZero(minute));
  Serial.print(F(":"));
  Serial.print(leadingZero(second));
  Serial.print(F(" "));
  if(day) // check for 12h
  {
    if(is12H) // checking for ampm
    {
      Serial.print(F("P"));
    }
    else
    {
      Serial.print(F("A"));
    }
    Serial.print(F("M"));
  }
  else
  {
    Serial.print(F("24H"));
  }
  Serial.print(F(" "));
  Serial.println(Weekdays(getWeekdays(day,month,year)));
}

void softRTC::print()
{
  uint8_t month, day, hour, minute, second; uint16_t year;
  calcTime(year, month, day, hour, minute, second);
  print(year, month, day, hour, minute, second,clkset.is12H);
}
