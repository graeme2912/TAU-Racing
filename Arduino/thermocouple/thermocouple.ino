// Sample Arduino MAX6675 Arduino Sketch

#include "max6675.h"

int ktcSO = 8;
int ktcCS = 9;
int ktcCLK = 10;

MAX6675 sensor1(ktcCLK, ktcCS, ktcSO);
MAX6675 sensor2(ktcCLK, 7, ktcSO);

void setup() {
  Serial.begin(9600);
  // give the MAX a little time to settle
  delay(500);
}

void loop() {
  // basic readout test
  
   Serial.print("Deg C Sensor 1 = "); 
   Serial.print(sensor1.readCelsius());
   //Serial.print("\t Deg F Sesnor 1 = ");
   //Serial.print(sensor1.readFahrenheit());

   Serial.print("\t Deg C Sensor 2 = "); 
   Serial.println(sensor2.readCelsius());
   //Serial.print("\t Deg F Sesnor 2 = ");
   //Serial.println(sensor2.readFahrenheit());
 
   delay(500);
}
