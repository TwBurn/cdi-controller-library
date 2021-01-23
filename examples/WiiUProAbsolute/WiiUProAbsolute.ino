/*
 * Example sketch for the CD-i Controller Library using a WiiU Pro controller (absolute mode, stick range is the full secreen)
 * By Jeffrey Janssen - cdi@nmotion.nl
 */
 
#include <Wii.h>
#include <usbhub.h>
#include <CdiController.h>

#define PIN_RTS 2
#define PIN_RXD 3 

#define JOY_MIN 1000
#define JOY_MAX 3000

USB Usb;
BTD Btd(&Usb);
//WII Wii(&Btd, PAIR); //For pairing controller
WII Wii(&Btd); //Normal usage

CdiController Cdi(PIN_RTS, PIN_RXD, ABSOLUTE);

void setup() {
	Serial.begin(115200);
	Usb.Init();
	Cdi.Init();
	Serial.println("Wii U Pro to CD-i started (Absolute Mode)");
}

void loop() {
	Usb.Task();
	Cdi.Task();
	
	//Check if we can update
	if(Wii.wiiUProControllerConnected) {
		//Analog Sticks
		int lhx = Wii.getAnalogHat(LeftHatX);
		int lhy = Wii.getAnalogHat(LeftHatY);
		int rhx = Wii.getAnalogHat(RightHatX);
		int rhy = Wii.getAnalogHat(RightHatY);

		lhx = constrain(lhx, JOY_MIN, JOY_MAX);
		lhy = constrain(lhy, JOY_MIN, JOY_MAX);
		rhx = constrain(rhx, JOY_MIN, JOY_MAX);
		rhy = constrain(rhy, JOY_MIN, JOY_MAX);
		
		int x = map(lhx, JOY_MIN, JOY_MAX, 0, 1023);
		int y = map(lhy, JOY_MAX, JOY_MIN, 0, 1023); //Inverted
		int dx = map(rhx, JOY_MIN, JOY_MAX, -64, 64);
		int dy = map(rhy, JOY_MAX, JOY_MIN, -64, 64); //Inverted

		x = constrain(x+dx, 0, 1023);
		y = constrain(y+dy, 0, 1023);
		
		//Buttons
		bool bt_1   = Wii.getButtonPress(A) || Wii.getButtonPress(Y);
		bool bt_2   = Wii.getButtonPress(B) || Wii.getButtonPress(X);
		bool p_down = Wii.getButtonPress(L3)|| Wii.getButtonPress(R3);//Stick click

		bool updated = Cdi.PenInput(x, y, bt_1, bt_2, p_down);

		if (updated) {
			Serial.print("x=");
			Serial.print(x);
			Serial.print(" y=");
			Serial.print(y);
			Serial.print(" B1=");
			Serial.print(bt_1);
			Serial.print(" B2=");
			Serial.print(bt_2);
			Serial.print(" PD=");
			Serial.print(p_down);
			Serial.println();
		}
  }
}
