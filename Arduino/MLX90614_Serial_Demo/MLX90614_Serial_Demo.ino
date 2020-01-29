/******************************************************************************
MLX90614_Serial_Demo.ino
Serial output example for the MLX90614 Infrared Thermometer

This example reads from the MLX90614 and prints out ambient and object
temperatures every half-second or so. Open the serial monitor and set the
baud rate to 9600.

Hardware Hookup (if you're not using the eval board):
MLX90614 ------------- Arduino
  VDD ------------------ 3.3V
  VSS ------------------ GND
  SDA ------------------ SDA (A4 on older boards)
  SCL ------------------ SCL (A5 on older boards)

An LED can be attached to pin 8 to monitor for any read errors.

Jim Lindblom @ SparkFun Electronics
October 23, 2015
https://github.com/sparkfun/SparkFun_MLX90614_Arduino_Library

Development environment specifics:
Arduino 1.6.5
SparkFun IR Thermometer Evaluation Board - MLX90614
******************************************************************************/

#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> // SparkFunMLX90614 Arduino library
/* Include the Hobby Components RTC library */
#include <HCRTC.h>
/* Include the adafruit modified SD card library, for compatability with the arduino mega */
#include <Adafruit_SD.h>


#define NUMBER_OF_SENSORS 4

unsigned int LOX_ADDRESSES[] = { 0x5A, 0x5B, 0x5C, 0x5D };

#define I2CDS1307Add 0x68

//enable extended serial output for debugging
bool verbose = true;
bool date_enable = true;

//infrared thermometer libraries
IRTherm therm[NUMBER_OF_SENSORS - 1]; // Create an IRTherm object to interact with throughout

//real time clock library
HCRTC HCRTC;

//SD Card library
File DataFile;

const byte LED_PIN = 8; // Optional LED attached to pin 8 (active low)

void enable_sensors() {
	if (verbose) Serial.print("enabling sensors");
	for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
		therm[i].begin(LOX_ADDRESSES[i]); //enable sensors
		therm[i].setUnit(TEMP_C); //set unit to degrees celcius
		Serial.print("Sensor enabled: ");
		Serial.print(i);
		Serial.print("   ");
		Serial.print(LOX_ADDRESSES[i]);
		Serial.println(" ");
	}
}

void write_sensor_data() {
	HCRTC.RTCRead(I2CDS1307Add);

	if (verbose) {
		Serial.print(HCRTC.GetDateString());
		Serial.print(",");
		Serial.print(HCRTC.GetTimeString());
		Serial.print(",");
	}

	DataFile = SD.open("DATALOG.CSV", FILE_WRITE);

	if (DataFile) {
		for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
			if (therm[i].read()) {
				if (verbose) {
					Serial.println(i);
					Serial.print("  Object: ");
					Serial.print(therm[i].object());
					Serial.print("   Ambient: ");
					Serial.print(therm[i].ambient());
					Serial.println();
				}
				DataFile.print(therm[i].object());
				delay(10);
			} else {
				if(verbose) Serial.println("failed");
				DataFile.print("ERR: NO READING");
			}
			DataFile.print(",");
		}
	} else {
		DataFile.write("test");
		Serial.print("failed");
	}

	DataFile.println();
	DataFile.close();
}


void setup()
{
	int SD_CS_DIO = 10;
	int SD_MOSI = 11;
	int SD_MISO = 12;
	int SD_SCK = 13;

	Serial.begin(9600); // Initialize Serial to log output
	Serial.print("starting");

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
	}
	else {
		Serial.println(F("SD Card OK"));
	}

	enable_sensors();
}

void loop_function(){
	if (verbose) {
		for (int j = 0; j < 20; j++) {
			Serial.println(" ");
		}
	}

	for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
		write_sensor_data();
		if (therm[i].read()) {
			if (verbose) {
				Serial.println(i);
				Serial.print("  Object: ");
				Serial.print(therm[i].object());
				Serial.print("   Ambient: ");
				Serial.print(therm[i].ambient());
				Serial.println();
			}
		} else {
			Serial.println("failed");
		}
	}
}

void loop()
{
	
	write_sensor_data();


	//setLED(LOW);
	delay(2000);
}



void setLED(bool on)
{
	if (on)
		digitalWrite(LED_PIN, LOW);
	else
		digitalWrite(LED_PIN, HIGH);
}
