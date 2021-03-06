#include"mcp_can.h"
#include <FastLED.h>
#define LED_PIN     6
#define NUM_LEDS    50
CRGB leds[NUM_LEDS];

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);

const int shift_points[5] = { 0, 9800, 9700, 9600, 9500 }; //still need to verify these with rory
const int number_of_gears = 4;
const int redline = 14000; //still need to verify after dyno

//globals for program operation
bool was_on;
int current_gear;
int current_rpm;

void setup() {
	//Serial.begin(115200);

	while (CAN_OK != CAN.begin(CAN_1000KBPS, MCP_8MHz))              // init can bus : baudrate = 1mbps
	{
		Serial.println("CAN BUS Shield init fail");
		Serial.println(" Init CAN BUS Shield again");
		delay(100);
	}
	Serial.println("CAN BUS Shield init ok!");

	CAN.init_Mask(0, 0, 0x03FF); //enable all filter bits
	CAN.init_Mask(1, 0, 0x03FF); //enable all filter bits

	CAN.init_Filt(0, 0, 0x600); //accept frame with identifier 0x600, frame 1 in the ecu config (contains the rpm data, slot 1) (also contains vbat, slot 3, which can be used for testing)                     
	//CAN.init_Filt(1, 0, 0x601); //accept frame with identifier 0x601, frame 2 in the ecu config (contains the tps data for testing, slot 4)               
	CAN.init_Filt(2, 0, 0x60E); //accept frame with identifier 0x601, frame 15 in the ecu config (contains current gear data, slot 2	or maybe slot 1)

	//CAN.init_Filt(3, 0, 0x07);                          // there are 6 filter in mcp2515
	//CAN.init_Filt(4, 0, 0x08);                          // there are 6 filter in mcp2515
	//CAN.init_Filt(5, 0, 0x09);                          // there are 6 filter in mcp2515

	Serial.println("CAN filters applied");

	//shift light setup
	FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
}

// double convert_tps(double input) {
// 	return (input / 81.92);
// }

// double convert_vbat(double input) {
// 	return (input / 1000);
// }

void flashLights(bool danger) {
	if (danger) {
		for (int i = 0; i <= NUM_LEDS; i++) {
			if (was_on)
				fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
			else
				//leds[i] = CRGB(0,0,0);
				fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0));
		}
	} else {
		for (int i = 0; i <= NUM_LEDS; i++) {
			if (was_on)
				fill_solid(leds, NUM_LEDS, CRGB(0, 0, 0));
			else
				//leds[i] = CRGB(0,0,0);
				fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));

		}
	}

	//delay(40);
	was_on = !was_on;
}

void lights(int leds_on) {

	for (int i = NUM_LEDS; i > leds_on; i--) {
		leds[i] = CRGB(0, 0, 0);
		FastLED.show();
	}

	//green, red, blue
	for (int i = 0; i <= leds_on; i++) {
		if (i < (3 * (NUM_LEDS / 3)))
			leds[i] = CRGB(0, 0, 255);

		if (i < (2 * (NUM_LEDS / 3)))
			leds[i] = CRGB(255, 0, 0);

		if (i < (NUM_LEDS / 3))
			leds[i] = CRGB(0, 255, 0);
	}
}

//old working version
// void updateLights(int rpm, int gear) {
// 	int leds_on;
// 	leds_on = map(rpm, 0, redline, 0, (1.4 * NUM_LEDS));

// 	Serial.println(rpm);
// 	Serial.println(leds_on);

// 	if (leds_on > NUM_LEDS) {
// 		if (leds_on > (1.2 * NUM_LEDS)) {
// 			flashLights(true);
// 		} else {
// 			flashLights(false);
// 		}
// 	} else {
// 		lights(leds_on);
// 	}

// 	FastLED.show();
// }

void updateLights(int rpm, int gear) {
	int leds_on;
	leds_on = map(rpm, 0, redline, 0, (1.4 * NUM_LEDS));
	Serial.println(rpm);
	Serial.println(leds_on);
	//NUM_LEDS is the total in the strip, the normal rpm range should use all of this
	//shift point should be just before this so it starts flashing just as it reaches it 
	//if the rpm is greater than the shift point, flash the lights to shift
	//if the rpm is within 500 rpm of the redline, flash red for danger i.e shift right now
	//if the rpm is below the shift point, operate as normal, with leds scaling linearly with rpm
	//still have to decide whether to start the lights at 0 rpm or not
	if(rpm > shift_points[gear]){
		if(rpm > (redline-500)){
			flashLights(true);
		} else {
			flashLights(false);
		}

	} else {
		lights(map(rpm, 0, shift_points[gear], 0, NUM_LEDS));
	}
	FastLED.show();
}


void loop() {
	unsigned char len = 0;
	unsigned char buf[8];

	if (CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
	{
		CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

		unsigned int canId = CAN.getCanId();

		Serial.println("-----------------------------");
		Serial.print("Get data from ID: ");
		Serial.println(canId, HEX);

		// if (canId == 0x601) {
		// 	// Store the coolant temp
        //     //coolantErr3Ref = (int16_t)((rxBuf[2] << 8) + rxBuf[3]);
		// 	// print throttle position
		// 	int tps_data = (int16_t)((buf[6] << 8) + buf[7]); //slot 4
		// 	Serial.print("throttle ref pre-conversion: ");
		// 	Serial.println(tps_data);
		// 	Serial.print("    Throttle ref post-conversion: ");
		// 	double throttlePosition = convert_tps((double)tps_data);
		// 	Serial.println(throttlePosition);

		// 	//shift light test
		// 	//please be fast
		// 	//update_lights(throttlePosition);
		// }

		if (canId == 0x600) {
			current_rpm = (int16_t)((buf[0] << 8) + buf[1]); //slot 1
			Serial.print("RPM: ");
			Serial.println(current_rpm);
		}

		if (canId == 0x60E) {
			current_gear = (int16_t)((buf[2] << 8) + buf[3]); //slot 2
			Serial.print("Gear: ");
			Serial.println(current_gear);
		}

		updateLights(current_rpm, current_gear);
		Serial.println();
	}
}