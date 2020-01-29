/* FILE     HCRTC_Example.cpp
   DATE:    23/12/13
   VERSION: 0.2
   AUTHOR:  Andrew Davies

This is an example of how to use the Hobby Components RTC library to read and
write to and from the DS1307 real time clock 24C32 EEPROM. The library is intended
to be used with our RTC clock module (HCMODU0011), but should work fine with any 
module that uses a DS1307 device.

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

/* Define the I2C addresses for the RTC and EEPROM */
#define I2CDS1307Add 0x68
#define I2C24C32Add  0x50

/* Example string to write to EEPROM */
char ExampleString[] = "Hobby Components";

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
  HCRTC.RTCWrite(I2CDS1307Add, 13, 4, 24, 14, 21, 0, 3);
}

void loop()
{
  /************* Example write to EEPROM ***************/  
  
  /* Initiate a write sequence. 
     Parameters are: I2C address, EEPROM start location */
  HCRTC.EEStartWrite(I2C24C32Add, 32000);
  
  for (index = 0; index < sizeof(ExampleString); index++)
  {  
    /* Sequentially write example data to the EEPROM */
    HCRTC.EEWriteByte(ExampleString[index]);
  }
  /* End the write sequence */
  HCRTC.EEEndWrite();
  
  /* Wait for cached data to finish writing */
  delay(10);  
  
  
  /************* Example read from EEPROM ***************/  
  
  /* Initiate read sequence.
     Parameters are: I2C address, EEPROM start location */
  HCRTC.EEStartRead(I2C24C32Add, 32000);
  

  for (index = 0; index < sizeof(ExampleString); index++)
  {
    /* Sequentially read data from EEPROM and output it to the UART */
    Serial.write(HCRTC.EEReadByte(I2C24C32Add));
  }    
  Serial.println();
    
 
  /* Continuously read the current time and date from the RTC */ 
  while(true)
  {
    /* Read the current time from the RTC module */
    HCRTC.RTCRead(I2CDS1307Add);
   
    /* Output the information to the UART */
    Serial.print(HCRTC.GetDay());
    Serial.print("/");
    Serial.print(HCRTC.GetMonth());
    Serial.print("/");
    Serial.print(HCRTC.GetYear());
    Serial.print(" ");
   
    Serial.print(HCRTC.GetHour());
    Serial.print(":");
    Serial.print(HCRTC.GetMinute());
    Serial.print(":");
    Serial.print(HCRTC.GetSecond());
    Serial.print(" DOW:");
    Serial.println(HCRTC.GetWeekday());
    
    /* Now output the same thing but using string functions instead: */
    Serial.print(HCRTC.GetDateString());
    Serial.print(" ");
    Serial.println(HCRTC.GetTimeString());
    
    /* Wait a second before reading again */
    delay(1000);  
  }
}