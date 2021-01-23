/*
 * Example sketch for the CD-i Controller Library using an analog stick, like the one included in an Arduino starter kit
 * By Jeffrey Janssen - cdi@nmotion.nl
 */
 
#include <CdiController.h>

#define PIN_RTS 2
#define PIN_RXD 3

#define PIN_X A0 //X direction
#define PIN_Y A1 //Y direction
#define PIN_B A2 //Button

#define JOY_MIN 0
#define JOY_MAX 1023

#define DEADZONE_AVG 512
#define DEADZONE_MAX (DEADZONE_AVG + 20)
#define DEADZONE_MIN (DEADZONE_AVG - 20)

CdiController Cdi(PIN_RTS, PIN_RXD, MANEUVER);

void setup() {
	Serial.begin(115200);
	Cdi.Init();
	Serial.println("Analog Stick to CD-i started");
}

//Joystick speed selection
byte JoySpeed = 32;

void loop() {
	Cdi.Task();
		
	//Analog Stick	
	int lhx = analogRead(PIN_X);
	int lhy = analogRead(PIN_Y);
	
	lhx = constrain(lhx, JOY_MIN, JOY_MAX);
	lhy = constrain(lhy, JOY_MIN, JOY_MAX);

	int x = map(lhx, JOY_MAX, JOY_MIN, -JoySpeed, +JoySpeed); //Inverted
	int y = map(lhy, JOY_MIN, JOY_MAX, -JoySpeed, +JoySpeed);

	if (lhx > DEADZONE_MIN && lhx < DEADZONE_MAX) x = 0;
	if (lhy > DEADZONE_MIN && lhy < DEADZONE_MAX) y = 0;
	
	//Buttons
	bool bt_1 = analogRead(PIN_B) == 0;
	bool bt_2 = false;

	bool updated = Cdi.JoyInput((byte)x, (byte)y, bt_1, bt_2);

	if (updated) {
		Serial.print("x=");
		Serial.print(x);
		Serial.print(" y=");
		Serial.print(y);
		Serial.print(" B1=");
		Serial.print(bt_1);
		Serial.print(" B2=");
		Serial.print(bt_2);
		Serial.println();
	}
}
