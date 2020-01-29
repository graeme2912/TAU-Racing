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

#define NUMBER_OF_SENSORS 5 //for some reason the last sensor always fails when logging or reading rtc so we need 5
#define I2CDS1307Add 0x68 //real time clock fixed address

unsigned int LOX_ADDRESSES[] = { 0x5A, 0x5B, 0x5C, 0x5D, 0x5E }; //addresses of each sensor
//unsigned int LOX_ADDRESSES[] = { 0x5B, 0x5A, 0x5C, 0x5D, 0x5E }; //addresses of each sensor (testing)


bool verbose = false; //enable extended serial output for debugging

IRTherm therm[NUMBER_OF_SENSORS - 1]; //Create multiple IRTherm objects to interact with throughout

HCRTC HCRTC; //real time clock library

File DataFile; //SD Card library

void enable_sensors() {
	float emis = 0.7; //emissivity of material 

	if (verbose) Serial.println("enabling sensors");
	for (int i = 0; i < NUMBER_OF_SENSORS; i++) {
	//for (int i = NUMBER_OF_SENSORS-1; i>-1; i--) { //testing

		delay(600);

		therm[i].begin(LOX_ADDRESSES[i]); //enable sensors
		//if (!therm[i].setEmissivity(emis)) while(1); //set emissivity co-efficient to emis

		//they don't like it when their emissivity is changed and then they are expected to run
		//change emissivity, then comment out this line, and re-run the program.
		//same thing with setting unit for the first one only, not entirely sure why

		//therm[i].setUnit(TEMP_C); //set unit to degrees celcius
		//therm[i].setEmissivity(emis);

		if(verbose){
			Serial.print("Sensor enabled: ");
			Serial.print(i);
			Serial.print("   at address: ");
			//Serial.print(LOX_ADDRESSES[i]);
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

void write_sensor_data() {
	HCRTC.RTCRead(I2CDS1307Add); //recieve real time clock data

	if (verbose) {
		Serial.print(HCRTC.GetDateString());
		Serial.print(",");
		Serial.print(HCRTC.GetTimeString());
		Serial.print(",");
	}

	DataFile = SD.open("BRAKE.CSV", FILE_WRITE);
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
	int count;
	for (int i = 0; i < (NUMBER_OF_SENSORS - 1); i++) {
		//if(i==0) delay(75); //extra delay for first one after printing time and date (unnecessary)
		if (therm[i].read()) {
			if (verbose) {
				Serial.println(i);
				Serial.print("  Object: ");
				Serial.print(therm[i].object());
				//Serial.print("   Ambient: ");
				//Serial.print(therm[i].ambient());
				//Serial.print(therm[i].object());

				Serial.println();
			}

			if(DataFile){	
				//if(therm[i].object() < 375) DataFile.print(therm[i].object()); else DataFile.print(" ");
				count = 0;
				while((therm[i].object() > 375) && (count < 4)) { 
					delay(25);
					if (count == 3) DataFile.print(-1);
					count++;
				}
				if(count < 3) DataFile.print(therm[i].object());
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