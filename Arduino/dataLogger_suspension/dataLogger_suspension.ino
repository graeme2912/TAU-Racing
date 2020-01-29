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
//#include <Wire.h>
/* Include the Hobby Components RTC library */
#include <HCRTC.h>
/* Include the adafruit modified SD card library, for compatability with the arduino mega */
#include <Adafruit_SD.h>

/* The RTC has a fixed addresses of 0x68 */
#define I2CDS1307Add 0x68

/* DIO pin used to control the SD card CS pin */
//#define SD_CS_DIO 10 


#include "Adafruit_VL53L0X.h"
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// address we will assign if dual sensor is present

#define NUMBER_OF_SENSORS 4

#define LIS3DH_CS 9 //pin for accelerometer

unsigned int LOX_ADDRESSES[] = { 0x30, 0x31, 0x32, 0x33 };

unsigned int SHT_LOX[] = { 7, 6, 5, 3};
// set the pins to shutdown

//enable extended serial output for debugging
bool verbose = false;



// objects for the vl53l0x
Adafruit_VL53L0X lox[NUMBER_OF_SENSORS - 1];

//for some reason this has to stay in for it to work, not sure why
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure[NUMBER_OF_SENSORS - 1];

/* Create an instance of HCRTC library */
HCRTC HCRTC;

/* Create an instance of the standard SD card library */
File DataFile;

//accelerometer object
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

/*
	Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
	Keep sensor #1 awake by keeping XSHUT pin high
	Put all other sensors into shutdown by pulling XSHUT pins low
	Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
	Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
	Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
 */

void reset_all(bool do_reset) {
	for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
		if (do_reset) {
			digitalWrite(SHT_LOX[i], LOW);
			if(verbose){
				Serial.print("Set sensor low: ");
				Serial.println(i);
			}			
		} else {
			digitalWrite(SHT_LOX[i], HIGH);
			if(verbose){
				Serial.print("Set sensor high: ");
				Serial.println(i);
			}	
		}
	}
	delay(10);
}

void enable_sensor(int num){
	digitalWrite(SHT_LOX[num], HIGH);
	if(verbose){
		Serial.print("Set sensor high: ");
		Serial.println(num);
	}	
	delay(10);

	//initing LOX
	while (!lox[num].begin(LOX_ADDRESSES[num])) {
		if (verbose) {
			Serial.print(F("Failed to boot VL53L0X number"));
			Serial.println(num);
			delay(100);
			//while (1);
		}
	}
}

void setID() {

	reset_all(true);
	reset_all(false);

	

	for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
		for(int j = i+1; j < NUMBER_OF_SENSORS; j++){
				digitalWrite(SHT_LOX[j], LOW);
				if(verbose){
					Serial.print("Set sensor low: ");
					Serial.println(j);
				}	
		}
		enable_sensor(i);
	 }	
}

void write_sensor_data() {
	bool debug = false;  // pass in 'true' to get debug data printout!
	for(int j = 0; j < NUMBER_OF_SENSORS; j++){
		lox[j].rangingTest(&measure[j], debug);
	}

	/* Read the current time from the RTC module */
	HCRTC.RTCRead(I2CDS1307Add);

	/* Lets output this data to the serial port */
	Serial.print(HCRTC.GetDateString());
	Serial.print(",");
	Serial.print(HCRTC.GetTimeString());
	Serial.print(",");

	/* Open the data.csv file to save our data to.
	   If the file already exists it will just tag our new data onto the end of it */
	DataFile = SD.open("SUSP.CSV", FILE_WRITE);

	if (DataFile)
	{
		DataFile.print(HCRTC.GetDay()); DataFile.print("-"); DataFile.print(HCRTC.GetMonth());
		DataFile.print(",");
		DataFile.print(HCRTC.GetTimeString());
		DataFile.print(",");

		for(int i = 0; i < NUMBER_OF_SENSORS; i++){
			if (measure[i].RangeStatus != 4) {     // if not out of range
				DataFile.print(measure[i].RangeMilliMeter);
				Serial.print(measure[i].RangeMilliMeter);
			} else {
				DataFile.print("error: range");
				Serial.print("error: range");
			}
			DataFile.print(",");
			Serial.print(",");
		}

		 lis.read();      // get X Y and Z data at once
		  // Then print out the raw data
		  /*
		  Serial.print("X:  "); Serial.print(lis.x); 
		  Serial.print("  \tY:  "); Serial.print(lis.y); 
		  Serial.print("  \tZ:  "); Serial.print(lis.z); 
		  */
		  /* Or....get a new sensor event, normalized */ 
		  sensors_event_t event; 
		  lis.getEvent(&event);
  
		  /* Display the results (acceleration is measured in m/s^2) */

		  float x=event.acceleration.x;
		  float y=event.acceleration.y;
		  float z=event.acceleration.z;

		  Serial.print(" X: "); Serial.print(x); 
		  Serial.print(" Y: "); Serial.print(y); 
		  Serial.print(" Z: "); Serial.print(z); 


		//for some reason data from suspension sensors is incorrect when printing accelerometer data to file, fine when printing to serial.

		 //DataFile.print(",");
		 //DataFile.print(" X: "); DataFile.print(x); DataFile.print(",");
	     //DataFile.print(" Y: "); DataFile.print(y); DataFile.print(",");
		// DataFile.print(" Z: "); DataFile.print(z); DataFile.print(",");

		// print sensor one reading to file

		DataFile.println(" ");
		Serial.println(" ");
		DataFile.close();
	} else {
		Serial.print("Error opening datafile");
	}
}


void setup()
{
 /* Use the RTCWrite library function to set the time and date. 
     Parameters are: I2C address, year, month, date, hour, minute, second, 
     day of week */
	//HCRTC.RTCWrite(I2CDS1307Add, 2019, 6, 26, 13, 29, 30, 3);

	int SD_CS_DIO = 10;
	int SD_MOSI = 11;
	int SD_MISO = 12;
	int SD_SCK = 13;

	/* Initialise the serial port */
	if(verbose) Serial.begin(9600);

	/* Set the SD card CS pin to an output */
	pinMode(SD_CS_DIO, OUTPUT);

	/* Use the RTCWrite library function to set the time and date.
	   Parameters are: I2C address, year, month, date, hour, minute, second,
	   day of week. You would normally only need to do this once */
	   //HCRTC.RTCWrite(I2CDS1307Add, 18, 10, 8, 15, 38, 0, 0);

	/* Initialise the SD card */
	if (!SD.begin(SD_CS_DIO, SD_MOSI, SD_MISO, SD_SCK))
	{
		/* If there was an error output this to the serial port and go no further */
		Serial.println(F("ERROR: SD card failed to initialise"));
		while (1);
	} else {
		Serial.println(F("SD Card OK"));
	}

   Serial.println("LIS3DH test!");
  
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
   // while (1);
  }
  Serial.println("LIS3DH found!");
  
  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!
  
  Serial.print("Range = "); Serial.print(2 << lis.getRange());  
  Serial.println("G");



	//spacing
	//DataFile = SD.open("datalog.csv", FILE_WRITE);
	//DataFile.println("\n\n\n\n\n");
	//DataFile.close();

	//initialize the shutdown pins as outputs
	for(int i = 0; i < NUMBER_OF_SENSORS; i++){
		pinMode(SHT_LOX[i], OUTPUT);
	}

	if(verbose) Serial.println(F("Shutdown pins inited..."));

	//write low to pins, enabling reset mode, might not actually be needed here as is done later
	for(int j = 0; j < NUMBER_OF_SENSORS; j++){
		digitalWrite(SHT_LOX[j], LOW);
	}

	if(verbose) Serial.println(F("Both in reset mode...(pins are low)"));

	Serial.println(F("Starting..."));
	delay(1000);
	setID();
}

/* Main Loop */
void loop()
{
	write_sensor_data();
	/* Wait half second before reading again */
	delay(100);
}
