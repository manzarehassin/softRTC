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



#include "softRTC.h"

bool softRTC::checkLeapYear(uint16_t& year)
{
  /*
  Must be divisible by 4, and not divisible by 100: Leap year,
  If the year is divisible by both 4 and 100, it's a leap year only if it's also divisible by 400.
  */
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

uint8_t softRTC::getDays(uint16_t& year, uint8_t month) 
{
  const uint8_t PROGMEM days_[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && checkLeapYear(year)) // Check for leap year
  {
    return 29;
  } 
  else 
  {
    return days_[month - 1];
  }
}

/* 
  Calculations for get week days:
  Zeller's congruence is an algorithm devised by Christian Zeller in the 19th century to calculate the day of the week for any Julian or Gregorian calendar date. It can be considered to be based on the conversion between Julian day and the calendar date.
  y0: This calculates the adjusted year. If the month is January or February, it subtracts 1 from the year to account for the fact that these months are considered the last two months of the previous year in this algorithm.
  x: This calculates the century code, which is a number based on the year that helps determine the day of the week.
  m0: This calculates the adjusted month, which is a number from 3 to 14, with March being 3, April being 4, and so on.
  d0: This calculates the day of the week, using the formula (day + x + m0*31/12) % 7 + 1. The result is a number from 1 to 7, where 1 represents Sunday, 2 represents Monday, and so on.
  returns 1-7, where 1= Sunday, 7 = Saturday.
*/
uint8_t softRTC::getWeekdays(uint8_t day, uint8_t month, uint16_t& year) 
{
  const int y0 = year - (month < 3);
  const int x = y0 + y0/4 - y0/100 + y0/400;
  const int m0 = month + 12 * (month < 3) - 2;
  const int d0 = (day + x + m0*31/12) % 7 + 1;
  return d0;
}

// Function to update the timekeeping variables
void softRTC::update()
{
  const uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;
  if ((currentMillis - previousMillis >= 1000) and (syncStatus_()))
  {
    const uint8_t diff = (currentMillis - previousMillis)/1000;
    uint8_t buffer1 = clk.second + diff;
    clk.second=buffer1;
    uint16_t year; // long year
    if (buffer1 >= 60) // second
    {
      buffer1 -= 60;
      clk.second = 0 + buffer1 ;
      clk.minute++;
      if (clk.minute >= 60) 
      {
        clk.minute = 0;
        hourValidator(buffer1,0); //read hour
        buffer1++;
        year=buffer1;
        hourValidator(buffer1,1); //write hour
        if ((year >= 24)) 
        {
          buffer1 = 0;
          hourValidator(buffer1,1); //write hour
          clk.day++;
          yearValidator(year, 0); //read year
          if (clk.day > getDays(year, clk.month) or (clk.day==0)) 
          {
            clk.day = 1;
            clk.month++;
            if (clk.month >= 13) 
            {
              clk.month = 1;
              year++;
              yearValidator(year, 1); //write year
            }
          }
          clk.week= getWeekdays(clk.day, clk.month, year);
        }
      }
    }
    previousMillis = currentMillis;
    if(diff >= 61)
    {
      clk.sync_=0;
    }
  }
}

/*
 Converts 24h time to 12h.
 Input Parameter:

 Hour : Hour in 24h format [0-23]
 
 Output Parameters:
 ======================
 1. Hour : 12h hour. [1-12] (Same variable used here to deliver the result)
 2. AM/PM : AM = 0, PM = 1
 */
void softRTC::Convert_To_12h(uint8_t& _Hour, bool& _AMPM)
{
  switch(_Hour)
  {
    case 01 ... 11: _AMPM = 0; break; // AM
    case 13 ... 23: _AMPM = 1; _Hour = _Hour - 12; break; // PM
    case 12       : _AMPM = 1; _Hour = 12; break; // PM
    default       : _AMPM = 0; _Hour = 12; break; // AM
  }
}

/*
 Converts 12h time to 24h.
 Input Parameters:
 ================================
 1. Hour   : Hour in 12h format (1-12)
 2. AMPM   : AM = 0, PM =1
 
 Output Parameter:

 Hour : 24h hour. (Same variable used here to deliver the result)
 */
void softRTC::Convert_To_24h(uint8_t& _Hour, bool & _AMPM)
{
  switch(_AMPM)
  {
    case 1: // PM
            if(_Hour < 12)
            {
              _Hour = _Hour + 12;  // 1 pm = 13
            }
            break;

    case 0: // AM
            if(_Hour == 12) //12 AM (Night)
            {
              _Hour = 0; // 24 or 00 hrs
            }
            break;
  }
}

void softRTC::hourValidator(uint8_t& hr, bool rw) // rw: 0= read, 1= write
{
  if(rw) //write hour
  {
    if(clk.mode12) // 12hr mode
    {
      //bool ampm;
      Convert_To_12h(hr, rw);
      clk.ampm = rw;
    }
    clk.hour = hr;
  }
  else //read hour in 24h mode
  {
    hr= clk.hour;
    if(clk.mode12)// 12h mode
    {
      //bool ampm = clk.ampm;
      rw=clk.ampm;
      Convert_To_24h(hr, rw);
    }
  }
}

void softRTC::yearValidator(uint16_t& year, bool rw) // rw: 0= read, 1= write
{
  uint8_t half;
  if(rw) // Write year in short mode
  {
    manageYear(rtc_short_year, year, half, clk.century);
    clk.year = half;
    if(year>=2100)  { clk.century = 1; }
    else            { clk.century = 0; }
  }
  else // Read year in full mode
  {
    rw = clk.century;
    half = clk.year;
    manageYear(rtc_full_year, year, half,rw); // convert to full mode
  }
}

void softRTC::manageYear(bool select, uint16_t& Full, uint8_t& Half, bool century)
{
  switch(select)
  {
    case rtc_short_year : Half = Full % 100; break;
    case rtc_full_year  : Full = (20 + century) * 100 + Half; break;
  }
}

void softRTC::write(uint8_t day, uint8_t month, uint16_t year, uint8_t hour, uint8_t minute, uint8_t second, bool ampm, bool mode12h) 
{
  struct var
  {
    bool error:1;
  }v1;
  v1.error=0;
  if((day>getDays(year, month)) or (day<=0)) { v1.error=1; } //day
  if((month>12) or (month<=0)) { v1.error=1; } // month
  if((minute>59) or (minute <0) or (second > 59) or (second<0)) { v1.error=1; } // minute, second
  if((mode12h==MODE_12H) and ((hour > 12) or (hour <= 0))) { v1.error=1; } // hour 12h
  if((mode12h==MODE_24H) and ((hour > 23) or (hour < 0))) { v1.error=1; } // hour 24h
  if(v1.error)
  {
    errorMsg(error_invalid_date);
  }
  else
  {
  clk.day = day;
  clk.month = month;
  yearValidator(year, 1);
  clk.week = getWeekdays(clk.day, clk.month,year);
  clk.mode12=mode12h;
  clk.minute = minute;
  clk.second = second;
  clk.hour = hour;
  clk.ampm=ampm;
  clk.sync_=1;
  }
}

void softRTC::read(uint8_t &day, uint8_t &month, uint16_t &year, uint8_t &hour, uint8_t &minute, uint8_t &second, bool & ampm, bool &mode12h, uint8_t &week)
{
  if(syncStatus_())
  {
    day = clk.day;
    month = clk.month;
    yearValidator(year, 0);
    hour = clk.hour;
    minute= clk.minute;
    second = clk.second;
    ampm = clk.ampm;
    mode12h = clk.mode12;
    week = clk.week;
  }
  else
  {
    day=month=hour=year=minute=second=ampm=mode12h=week=0;
  }
}

String softRTC::getNames(uint8_t val)
{
    switch(val)
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

// Function to print the current time
void softRTC::print()
{
  if(syncStatus_())
  {
  uint16_t f_year;
  uint8_t  s_year=clk.year;
  manageYear(rtc_full_year, f_year, s_year, clk.century);
  Serial.print(clk.day);
  Serial.print(F("-"));
  Serial.print(clk.month);
  Serial.print(F("-"));
  Serial.print(f_year);
  Serial.print(F(" "));

  Serial.print(leadingZero(clk.hour));
  Serial.print(F(":"));
  Serial.print(leadingZero(clk.minute));
  Serial.print(F(":"));
  Serial.print(leadingZero(clk.second));
  Serial.print(F(" "));
  if(clk.mode12)
  {
    if(clk.ampm)
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
  Serial.println(getNames(clk.week));
  }
}

String softRTC::leadingZero(uint8_t value)
{
  if((value<10) or (!value))
  {
    return String(F("0")) + String(value);
  }
  else
  {
    return String(value);
  }
}

bool softRTC::syncStatus_()
{
  const bool x= (clk.enable_ and clk.sync_);
  if (!x)
  {
    errorMsg(error_not_sync);
  }
  return x;
}

void softRTC::errorMsg(uint8_t val)
{
  #define msg_inv   F("Error: Invalid date & time.")
  #define msg_sync  F("Error: Not synced yet.")

  switch(val)
  {
    case error_not_sync: Serial.println(msg_sync); break;
    case error_invalid_date: Serial.println(msg_inv); break;
  };
}