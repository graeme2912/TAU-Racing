#include<Wire.h>
#include<Adafruit_SD.h>
#include<MPU6050.h>

MPU6050 accel;

void setup_SD_card() {
	int SD_CS_DIO = 10;
	int SD_MOSI = 11;
	int SD_MISO = 12;
	int SD_SCK = 13;

	pinMode(SD_CS_DIO, OUTPUT);
	

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
}

void setup() {
	Serial.begin(9600);
	Serial.print("Starting");

	setup_SD_card();
}

void loop() {

}