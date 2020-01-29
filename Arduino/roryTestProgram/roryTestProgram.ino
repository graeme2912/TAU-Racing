int firstSolenoidPin = 7;
int secondSolenoidPin = 8;
int counter = 1;

void setup() {
	Serial.begin(9600);
	pinMode(firstSolenoidPin, OUTPUT);
	pinMode(secondSolenoidPin, OUTPUT);

}

void loop() {
	//fireSolenoid(150, secondSolenoidPin); //delay in ms
	fireBothSolenoids(150, 150, firstSolenoidPin, secondSolenoidPin); //delays both in ms
	Serial.print("The solenoids have fired: ");
	Serial.print(counter);
	Serial.println(" time(s).");
	counter++;
}

void fireBothSolenoids(int ms, int between, int pin1, int pin2){
	fireSolenoid(ms, pin1);
	delay(between);
	fireSolenoid(ms, pin2);
	delay(between);
}

void fireSolenoid(int ms, int solenoidPin) {
	digitalWrite(solenoidPin, HIGH);
	delay(ms);
	digitalWrite(solenoidPin, LOW);
	delay(ms);
}