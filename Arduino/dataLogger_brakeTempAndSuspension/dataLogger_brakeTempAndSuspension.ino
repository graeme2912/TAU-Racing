/******************************************************************************
Hardware Hookup (if you're not using the eval board):
MLX90614 ------------- Arduino
  VDD ------------------ 3.3V
  VSS ------------------ GND
  SDA ------------------ SDA (A4 on older boards)
  SCL ------------------ SCL (A5 on older boards)
******************************************************************************/

#include <Wire.h> // I2C library, required for MLX90614
#include <SparkFunMLX90614.h> // SparkFunMLX90614 Arduino library
#include <HCRTC.h> //Include the Hobby Components RTC library
#include <Adafruit_SD.h> //Include the adafruit modified SD card library, for compatability with the arduino mega
#include "Adafruit_VL53L0X.h" //time of flight sensor library
#include <Adafruit_Sensor.h>

#define NUMBER_OF_THERM_SENSORS 5 //for some reason the last sensor always fails when logging or reading rtc so we need 5
#define NUMBER_OF_SUSP_SENSORS 4
#define I2CDS1307Add 0x68 //real time clock fixed address

unsigned int THERM_ADDRESSES[] = { 0x5A, 0x5B, 0x5C, 0x5D, 0x5E }; //addresses of each sensor
unsigned int LOX_ADDRESSES[] = { 0x30, 0x31, 0x32, 0x33 }; //suspension sensors addressses
unsigned int SHT_LOX[] = { 7, 6, 5, 3 }; //shutdown pins

bool verbose = false; //enable extended serial output for debugging

IRTherm therm[NUMBER_OF_THERM_SENSORS - 1]; //Create multiple IRTherm objects to interact with throughout
											// objects for the vl53l0x
Adafruit_VL53L0X lox[NUMBER_OF_SUSP_SENSORS - 1];

//for some reason this has to stay in for it to work, not sure why
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measure[NUMBER_OF_SUSP_SENSORS - 1];

HCRTC HCRTC; //real time clock library

File DataFile; //SD Card library

void enable_sensors() {
	float emis = 0.7; //emissivity of material 

	setup_suspension_sensors();


	if (verbose) Serial.println("enabling sensors");
	for (int i = 0; i < NUMBER_OF_THERM_SENSORS; i++) {
		therm[i].begin(THERM_ADDRESSES[i]); //enable sensors
		therm[i].setUnit(TEMP_C); //set unit to degrees celcius
		//if (!therm[i].setEmissivity(emis)) while(1); //set emissivity co-efficient to emis
		
		//they don't like it when their emissivity is changed and then they are expected to run
		//change emissivity, then comment out this line, and re-run the program.
		//therm[i].setEmissivity(emis);

		if(verbose){
			Serial.print("Sensor enabled: ");
			Serial.print(i);
			Serial.print("   at address: ");
			//Serial.print(THERM_ADDRESSES[i]);
			Serial.print(therm[i].readAddress()); //will return 0 of the sensor fails to initialize
			Serial.print(" with emissivity: ");
			Serial.print(therm[i].readEmissivity()); //will return 0 of the sensor fails to initialize
			Serial.println();
		}
	delay(200);
	}

}

void setup_SD_card() {
	//generic sd card setup function, reusable
	int SD_CS_DIO = 10;
	int SD_MOSI = 11;
	int SD_MISO = 12;
	int SD_SCK = 13;

	pinMode(SD_CS_DIO, OUTPUT);
	/* Initialise the SD card */
	if (!SD.begin(SD_CS_DIO, SD_MOSI, SD_MISO, SD_SCK)) {
		/* If there was an error output this to the serial port and go no further */
		Serial.println(F("ERROR: SD card failed to initialise"));
		while (1);
	} else {
		Serial.println(F("SD Card OK"));
	}
}

void reset_all(bool do_reset) {
	for (int i = 0; i < NUMBER_OF_SUSP_SENSORS; i++) {
		if (do_reset) {
			digitalWrite(SHT_LOX[i], LOW);
			if (verbose) {
				Serial.print("Set sensor low: ");
				Serial.println(i);
			}
		}
		else {
			digitalWrite(SHT_LOX[i], HIGH);
			if (verbose) {
				Serial.print("Set sensor high: ");
				Serial.println(i);
			}
		}
	}
	delay(10);
}

void enable_sensor(int num) {
	digitalWrite(SHT_LOX[num], HIGH);
	if (verbose) {
		Serial.print("Set sensor high: ");
		Serial.println(num);
	}
	delay(100);

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

	for (int i = 0; i < NUMBER_OF_SUSP_SENSORS; i++) {
		for (int j = i + 1; j < NUMBER_OF_SUSP_SENSORS; j++) {
			digitalWrite(SHT_LOX[j], LOW);
			if (verbose) {
				Serial.print("Set sensor low: ");
				Serial.println(j);
			}
		}

		enable_sensor(i);

	}
}

void setup_suspension_sensors() {
	for (int i = 0; i < NUMBER_OF_SUSP_SENSORS; i++) {
		pinMode(SHT_LOX[i], OUTPUT);
	}

	if (verbose) Serial.println(F("Shutdown pins inited..."));

	//write low to pins, enabling reset mode, might not actually be needed here as is done later
	for (int j = 0; j < NUMBER_OF_SUSP_SENSORS; j++) {
		digitalWrite(SHT_LOX[j], LOW);
	}

	if (verbose) Serial.println(F("Both in reset mode...(pins are low)"));

	Serial.println(F("Starting..."));
	delay(1000);
	setID();
}


void write_sensor_data() {
	HCRTC.RTCRead(I2CDS1307Add); //recieve real time clock data

	bool debug = false;  // pass in 'true' to get debug data printout!
	for (int j = 0; j < NUMBER_OF_SUSP_SENSORS; j++) {
		lox[j].rangingTest(&measure[j], debug);
	}

	if (verbose) {
		Serial.print(HCRTC.GetDateString());
		Serial.print(",");
		Serial.print(HCRTC.GetTimeString());
		Serial.print(",");
	}

	DataFile = SD.open("DATALOG.CSV", FILE_WRITE);
	if (verbose) {
		if(DataFile) Serial.print("opened file");
	}

	if (DataFile) {
		DataFile.print(HCRTC.GetDateString());
		DataFile.print(",");
		DataFile.print(HCRTC.GetTimeString());
		DataFile.print(",");
	}

	delay(25); //program restars if this delay does not exist, 25 was as short as i could get it 

	// (i < (NUMBER_OF_SENSORS - 1))   because we have to run one more sensor than required to ensure the code works correctly, but dont bother trying to read because it will fail
	for (int i = 0; i < (NUMBER_OF_THERM_SENSORS - 1); i++) {
		if (therm[i].read()) {
			if (verbose) {
				Serial.println(i);
				Serial.print("  Object: ");
				Serial.print(therm[i].object());
				Serial.print("   Ambient: ");
				Serial.print(therm[i].ambient());
				Serial.println();
			}

			if(DataFile){	
				DataFile.print(therm[i].object());
			} else {
				if(verbose) Serial.print("failed to write to file");
			}
		} else {
			if(verbose){
				Serial.println("failed to read sensor data");
				DataFile.print("ERR: NO READING");
			}
		}
		DataFile.print(",");
	}

	for (int k = 0; k < NUMBER_OF_SUSP_SENSORS; k++) {
		if (measure[k].RangeStatus != 4) {     // if not out of range
			DataFile.print(measure[k].RangeMilliMeter);
			if(verbose) Serial.print(measure[k].RangeMilliMeter);
		}
		else {
			if (verbose) {
				DataFile.print("error: range");
				Serial.print("error: range");
			}
		}
		DataFile.print(",");
		if(verbose) Serial.print(",");
	}

	DataFile.println();
	DataFile.close();
}

void setup() {
	Serial.begin(9600); // Initialize Serial to log output
	Serial.print("starting");
	enable_sensors();
	setup_SD_card();
}

void loop() {
	write_sensor_data();
}