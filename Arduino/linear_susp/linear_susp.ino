#include <HCRTC.h>
#include <Adafruit_SD.h>
HCRTC HCRTC;
File DataFile;
#define I2CDS1307Add 0x68



void setup(){
	pinMode(A0, INPUT);
	pinMode(A1, INPUT);
	pinMode(A2, INPUT);
	pinMode(A3, INPUT);
	Serial.begin(9600);


	int SD_CS_DIO = 10;
	int SD_MOSI = 11;
	int SD_MISO = 12;
	int SD_SCK = 13;
	pinMode(SD_CS_DIO, OUTPUT);
	if (!SD.begin(SD_CS_DIO, SD_MOSI, SD_MISO, SD_SCK)) {
		/* If there was an error output this to the serial port and go no further */
		Serial.println(F("ERROR: SD card failed to initialise"));
		while (1);
	} else {
		Serial.println(F("SD Card OK"));
	}

}
int val0, val1, val2, val3 = 0;


void loop() {
	val0 = analogRead(A0);
	val1 = analogRead(A1);
	val2 = analogRead(A2);
	val3 = analogRead(A3);


	HCRTC.RTCRead(I2CDS1307Add);
	if (DataFile) {
		DataFile.print(HCRTC.GetDay()); DataFile.print("-"); DataFile.print(HCRTC.GetMonth());
		DataFile.print(",");
		DataFile.print(HCRTC.GetTimeString());
		DataFile.print(",");
		DataFile.print(val0);
		DataFile.print(",");
		DataFile.print(val1);
		DataFile.print(",");
		DataFile.print(val2);
		DataFile.print(",");
		DataFile.println(val3);

		DataFile.close();
	} else {
		Serial.print("Error opening datafile");
	}

}