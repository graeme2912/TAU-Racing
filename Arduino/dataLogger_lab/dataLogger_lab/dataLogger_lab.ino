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

#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

/* The RTC has a fixed addresses of 0x68 */
#define I2CDS1307Add 0x68

/* DIO pin used to control the SD card CS pin */
#define SD_CS_DIO 10 

/* Create an instance of HCRTC library */
HCRTC HCRTC;

/* Create an instance of the standard SD card library */
File DataFile;

//accelerometer config
// Used for software SPI
#define LIS3DH_CLK 7 //13,12,11,10
#define LIS3DH_MISO 6
#define LIS3DH_MOSI 5
// Used for hardware & software SPI
#define LIS3DH_CS 9

// software SPI
//Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS, LIS3DH_MOSI, LIS3DH_MISO, LIS3DH_CLK);
// hardware SPI
Adafruit_LIS3DH lis = Adafruit_LIS3DH(LIS3DH_CS);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
   #define Serial SerialUSB
#endif

//thermocouple


#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

MAX6675 sensor1(ktcCLK, ktcCS, ktcSO);
MAX6675 sensor2(ktcCLK, 7, ktcSO);
MAX6675 sensor3(ktcCLK, 3, ktcSO);

// This code exercises the MPX7002DP  
// Pressure sensor connected to A0  
int sensorPin = A0;  // select the input pin for the Pressure Sensor  
int sensorValue = 0;  // variable to store the Raw Data value coming from the sensor  
float outputValue = 0; // variable to store converted kPa value   

void setup()
{
  pinMode(sensorPin, INPUT);  // Pressure sensor is on Analogue pin 0 
  #ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
  #endif

  /* Initialise the serial port */
  Serial.begin(9600);
  
  /* Set the SD card CS pin to an output */ 
  pinMode(SD_CS_DIO, OUTPUT);
  
  /* Use the RTCWrite library function to set the time and date. 
     Parameters are: I2C address, year, month, date, hour, minute, second, 
     day of week. You would normally only need to do this once */
  HCRTC.RTCWrite(I2CDS1307Add, 18, 10, 04, 15, 45, 0, 4);

 /*  Serial.println("LIS3DH test!");
  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");
  
   Initialise the SD card */
  if (!SD.begin(SD_CS_DIO)) 
  {
     //If there was an error output this to the serial port and go no further 
    Serial.println("ERROR: SD card failed to initialise");
    while(1);
  }else
  {
    Serial.println("SD Card OK");
  }
}



/* Main Loop */
void loop()
{
    
  /* Read the current time from the RTC module */
  HCRTC.RTCRead(I2CDS1307Add);
    
  /* Lets output this data to the serial port */ 
  Serial.print(HCRTC.GetDateString());
  Serial.print(", ");
  Serial.print(HCRTC.GetTimeString());
  Serial.print(", ");
 // Serial.println(Temperature);    

  //lis.read();      // get X Y and Z data at once
  // Then print out the raw data
  /*
  Serial.print("X:  "); Serial.print(lis.x); 
  Serial.print("  \tY:  "); Serial.print(lis.y); 
  Serial.print("  \tZ:  "); Serial.print(lis.z); 
*/
  /* Or....get a new sensor event, normalized */ 
  //sensors_event_t event; 
  //lis.getEvent(&event);

  sensorValue = analogRead(sensorPin);  
  // map the Raw data to kPa  
  outputValue = map(sensorValue, 0, 1023, -2000, 2000); 
  
  /* Display the results (acceleration is measured in m/s^2) */
  
//  float x=event.acceleration.x;
 // float y=event.acceleration.y;
  //float z=event.acceleration.z;

  //sensor values
  double sensor1_value = sensor1.readCelsius();
  double sensor2_value = sensor2.readCelsius();
  double sensor3_value = sensor3.readCelsius();
  
  /*Serial.print(" X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y); 
  Serial.print(" Z: "); Serial.print(z); 
  */
  Serial.print(" sensor1: "); Serial.print(sensor1_value);
  Serial.print(" sensor2: "); Serial.print(sensor2_value);
  Serial.print(" sensor3: "); Serial.print(sensor3_value);
  Serial.print(" pressure sensor: "); Serial.print(outputValue);
  Serial.println(" m/s^2 ");
    
  /* Open the data.csv file to save our data to. 
     If the file already exists it will just tag our new data onto the end of it */
  DataFile = SD.open("data.csv", FILE_WRITE);

  if (DataFile) 
  {
    DataFile.print(HCRTC.GetDateString());
    DataFile.print(", ");
    DataFile.print(HCRTC.GetTimeString());
    DataFile.print(", ");
    /*DataFile.print(x);
    DataFile.print(",");
    DataFile.print(y);
    DataFile.print(",");
    DataFile.print(z);
    DataFile.print(",");
    */
    DataFile.print(sensor1_value);
    DataFile.print(",");
    DataFile.print(sensor2_value);  
    DataFile.print(",");
    DataFile.print(sensor3_value); 
    DataFile.print(",");
    DataFile.println(outputValue); 
    DataFile.close();
  }

  /* Wait a second before reading again */
  delay(500);
}
