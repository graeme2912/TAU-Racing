/* FILE     HCRTC_LCD_Example.cpp
   DATE:    23/12/13
   VERSION: 0.1
   AUTHOR:  Andrew Davies

This is an example of how to use the Hobby Components RTC library to output the time 
and date to an LCD display. The library is intended to be used with our RTC clock module 
(HCMODU0011), but should work fine with any module that uses a DS1307 device.

You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.
This software may not be used directly for the purpose of selling products that
directly compete with Hobby Components Ltd's own range of products.

THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR LACK OF NEGLIGENCE.
HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR ANY DAMAGES,
INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY
REASON WHATSOEVER.
*/


/* Include the wire library */
#include "Wire.h"
/* Include the Hobby Components RTC library */
#include <HCRTC.h>

/* Include the standard LiquidCrystal library */
#include <LiquidCrystal.h>

/* Define the I2C addresses for the RTC and EEPROM */
#define I2CDS1307Add 0x68

/* DIO pin definitions */
#define LCD_DATA4 4         /* LCD data DIO pin 4 */
#define LCD_DATA5 5         /* LCD data DIO pin 5 */
#define LCD_DATA6 6         /* LCD data DIO pin 6 */
#define LCD_DATA7 7         /* LCD data DIO pin 7 */
#define LCD_RESET 8         /* LCD Reset DIO pin */
#define LCD_ENABLE 9        /* LCD Enable DIO pin */
#define LCD_BACKLIGHT 10    /* LCD backlight DIO pin */


/* Initialise the LiquidCrystal library with the correct DIO pins */
LiquidCrystal lcd(LCD_RESET, LCD_ENABLE, LCD_DATA4, LCD_DATA5, LCD_DATA6, LCD_DATA7);


/* Used in example as an index pointer */
byte index;

/* Create an instance of HCRTC library */
HCRTC HCRTC;


void setup()
{
  Serial.begin(9600);
  
  /* Use the RTCWrite library function to set the time and date. 
     Parameters are: I2C address, year, month, date, hour, minute, second, 
     day of week */
  HCRTC.RTCWrite(I2CDS1307Add, 13, 12, 23, 14, 21, 0, 3);
  
   /* Set the correct display size (16 character, 2 line display) */
  lcd.begin(16, 2); 
}

void loop()
{
  /* Read the current time from the RTC module */
  HCRTC.RTCRead(I2CDS1307Add);
    
  /* Output current date to the LCD */
  lcd.setCursor(0,0); 
  lcd.print(HCRTC.GetDateString());
  
  /* Output current time to the LCD */
  lcd.setCursor(0,1); 
  lcd.print(HCRTC.GetTimeString());
}

