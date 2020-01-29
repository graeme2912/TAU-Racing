#include <FastLED.h>
#define LED_PIN     7
#define NUM_LEDS    50
#define INPUT_PIN A0
CRGB leds[NUM_LEDS];

bool was_on;

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(INPUT_PIN, INPUT);
}

void loop() {
  int leds_on;
  leds_on = map(analogRead(INPUT_PIN), 0, 1023, 0, (1.4*NUM_LEDS));

  Serial.println(analogRead(INPUT_PIN));
  Serial.println(leds_on);

  if (leds_on > NUM_LEDS) {
	  if (leds_on > (1.2 * NUM_LEDS)) {
		  flashLights(true);
	  }
	  else {
		  flashLights(false);
	  }
  }  else {
	  lights(leds_on);
  }
  //flashLights();

  FastLED.show();
}

void flashLights(bool danger){
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

  delay(40);
  was_on = !was_on;
}

void lights(int leds_on){

  for(int i = NUM_LEDS; i > leds_on; i--){
	leds[i] = CRGB(0,0,0);
	  FastLED.show();
  }

  for(int i = 0; i <= leds_on; i++){
	if (i < (3 * (NUM_LEDS / 3)))
	  leds[i] = CRGB(0, 0, 255);

	if(i < (2*(NUM_LEDS/3))) 
	  leds[i] = CRGB ( 0, 255, 0);

	if(i < (NUM_LEDS/3))
	   leds[i] = CRGB ( 255, 0, 0);
  }
}

