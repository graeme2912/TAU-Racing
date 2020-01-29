/* FILE:    Data_Logger_Shield_HCARDU0093_Example
   DATE:    24/02/15
   VERSION: 0.1
   
REVISIONS:

24/02/15 Created version 0.1

This is an example of how to use the Hobby Components data logger shield 
(HCMODU0093). This shield contains a battery backed DS1307 real time clock
and an SD card interface. This allows any sensor data read by your Arduino 
to be stored with an accurate time stamp to an SD card. In this example sketch
we will continually read the value of an LM35 temperature sensor and store the
result together with the current time and date to a CSV file. This file can then 
be loaded into a spreadsheet such as Excel or Open Office.

To use this sketch you will require the HCTRC library which is available for
download in the software section of our support forum. You will also need to
connect an LM35 temperature sensor as follows:

LM35.....Arduino
Pin 1....+5V
Pin 2....Analogue pin A0
Pin 3....GND

You may copy, alter and reuse this code in any way you like, but please leave
reference to HobbyComponents.com in your comments if you redistribute this code.

This software may not be used directly for the purpose of promoting products that
directly compete with Hobby Components Ltd's own range of products.

THIS SOFTWARE IS PROVIDED "AS IS". HOBBY COMPONENTS MAKES NO WARRANTIES, 
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ACCURACY OR
LACK OF NEGLIGENCE. HOBBY COMPONENTS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE
FOR ANY DAMAGES INCLUDING, BUT NOT LIMITED TO, SPECIAL, INCIDENTAL OR 
CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER. */



/* Include the wire library */
#include <Wire.h>
/* Include the Hobby Components RTC library */
#include <HCRTC.h>
/* Include the standard SD card library */
#include <SD.h>

/* The RTC has a fixed addresses of 0x68 */
#define I2CDS1307Add 0x68

/* DIO pin used to control the SD card CS pin */
#define SD_CS_DIO 10 

/* Define the analogue pin used to read the temperature sensor (A0) */
#define LM35Pin 0

//thermocouple -----------------------------------------
#include "max6675.h"

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;

/* Create an instance of HCRTC library */
HCRTC HCRTC;

/* Create an instance of the standard SD card library */
File DataFile;

/* This will store the current temperature reading */
float Temperature;


void setup()
{
  /* Initialise the serial port */
  Serial.begin(9600);

  /* Set internal 2.56V reference for analogue pins */
  analogReference(INTERNAL); 
  
  /* Set the SD card CS pin to an output */ 
  pinMode(SD_CS_DIO, OUTPUT);
  
  /* Use the RTCWrite library function to set the time and date. 
     Parameters are: I2C address, year, month, date, hour, minute, second, 
     day of week. You would normally only need to do this once */
  //HCRTC.RTCWrite(I2CDS1307Add, 18, 10, 8, 15, 38, 0, 0);
  
  /* Initialise the SD card */
  if (!SD.begin(SD_CS_DIO)) 
  {
    /* If there was an error output this to the serial port and go no further */
    Serial.println("ERROR: SD card failed to initialise");
    while(1);
  }else
  {
    Serial.println("SD Card OK");
  }

  //set up thermocouple
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
}



/* Main Loop */
void loop()
{
  /* Read the LM35 connected to the analogue pin and convert to oC */
  Temperature = thermocouple.readCelsius();//analogRead(LM35Pin) / 9.31;
    
  /* Read the current time from the RTC module */
  HCRTC.RTCRead(I2CDS1307Add);
    
  /* Lets output this data to the serial port */ 
  Serial.print(HCRTC.GetDateString());
  Serial.print(", ");
  Serial.print(HCRTC.GetTimeString());
  Serial.print(", ");
  Serial.println(Temperature);    
    
  /* Open the data.csv file to save our data to. 
     If the file already exists it will just tag our new data onto the end of it */
  DataFile = SD.open("data.csv", FILE_WRITE);

  if (DataFile) 
  {
    DataFile.print(HCRTC.GetDateString());
    DataFile.print(", ");
    DataFile.print(HCRTC.GetTimeString());
    DataFile.print(", ");
    DataFile.println(Temperature);  
    DataFile.close();
  }

  /* Wait a second before reading again */
  delay(1000);
}
