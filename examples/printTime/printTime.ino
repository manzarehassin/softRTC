/*
softRTC Example Sketch


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

softRTC rtc; // Creating object of softRTC class

void setup() 
{
  Serial.begin(9600); // Serial port initialized.

  /* Set the time: Day, Month, Year, Hour, Minute, Second,AM/PM, Hour Mode
  rtc.write(31, 12, 2024, 13, 10, 30,0,MODE_24H);  // 24hrs, 31 Dec, 2024 13:10:30
  rtc.write(31, 12, 2024, 11, 10, 30,AM,MODE_12H); // 12hrs, 31 Dec, 2024 11:10:30 AM
  rtc.write(31, 12, 2024, 11, 10, 30,PM,MODE_12H); // 12hrs, 31 Dec, 2024 11:10:30 PM
  */

  rtc.write(31, 12, 2024, 12, 59, 00,PM,MODE_12H); // 12hrs, 31 Dec, 2024 12:59:00 PM
  rtc.begin(); // Start the RTC
}

void loop() 
{
  rtc.update(); // To run the clock properly, call this update function inside loop. Mandatory.
  //rtc.print();  // Print the date & time in serial monitor. Builtin function of softRTC library
  extra_info(); // uncomment this to show similar result with individual components.
  delay(1000);  // Wait for a second.
}


void extra_info()
{
  uint8_t day, month, hour, minute, second, tmp1;
  uint16_t year;
  bool tmp;

  rtc.read(day,month,year,hour,minute,second,tmp,tmp,tmp1); // reading rtc: day,month,year,hour,minute,second,ampm,12h mode,week
  String ampm = rtc.ampm(); // am or pm
  String week = rtc.week(); // weekdays name

  Serial.print(day); Serial.print("-");
  Serial.print(month); Serial.print("-");
  Serial.print(year); Serial.print(" ");
  Serial.print(hour); Serial.print(":");
  Serial.print(rtc.leadingZero(minute)); Serial.print(":");
  Serial.print(rtc.leadingZero(second)); Serial.print(" ");
  Serial.print(ampm); Serial.print(" ");
  Serial.println(week);
}