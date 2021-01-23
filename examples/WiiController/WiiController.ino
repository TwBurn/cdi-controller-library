/*
 * Example sketch for the CD-i Controller Library using a WiiMote or Wii U Pro controller
 * By Jeffrey Janssen - cdi@nmotion.nl
 */
 
#include <Wii.h>
#include <usbhub.h>
#include <CdiController.h>

#define PIN_RTS 2
#define PIN_RXD 3 

#define JOY_MIN 1000
#define JOY_MAX 3000

#define DEADZONE_AVG 2000
#define DEADZONE_MAX (DEADZONE_AVG + 200)
#define DEADZONE_MIN (DEADZONE_AVG - 200)

USB Usb;
BTD Btd(&Usb);
//WII Wii(&Btd, PAIR); //For pairing controller
WII Wii(&Btd); //Normal usage

CdiController Cdi(PIN_RTS, PIN_RXD, MANEUVER);

void setup() {
	Serial.begin(115200);
	Usb.Init();
	Cdi.Init();
	Serial.println("WiiMote/Wii U Pro to CD-i started");
}

//Joystick speed selection
byte speed = 0;
int dSpeed[] = {4, 8, 16, 32};  //Digital (D-pad)
int aSpeed[] = {8, 16, 32, 64}; //Analog (Stick)

void loop() {
	Usb.Task();
	Cdi.Task();

	//Disconnect on HOME
	if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
		Serial.println("Disconnecting Controller");
		Wii.disconnect();
	}
	
	//Alter Speed
	if (Wii.getButtonClick(PLUS))  setSpeed(+1);
	if (Wii.getButtonClick(MINUS)) setSpeed(-1);
	
	//Analog Stick
	
	int lhx = Wii.getAnalogHat(LeftHatX);
	int lhy = Wii.getAnalogHat(LeftHatY);
	//int rhx = Wii.getAnalogHat(RightHatX);
	//int rhy = Wii.getAnalogHat(RightHatY);
	
	lhx = constrain(lhx, JOY_MIN, JOY_MAX);
	lhy = constrain(lhy, JOY_MIN, JOY_MAX);
	//rhx = constrain(rhx, JOY_MIN, JOY_MAX);
	//rhy = constrain(rhy, JOY_MIN, JOY_MAX);
	
	int x = map(lhx, JOY_MIN, JOY_MAX, -aSpeed[speed], +aSpeed[speed]);
	int y = map(lhy, JOY_MAX, JOY_MIN, -aSpeed[speed], +aSpeed[speed]); //Inverted
	
	if (lhx > DEADZONE_MIN && lhx < DEADZONE_MAX) x = 0;
	if (lhy > DEADZONE_MIN && lhy < DEADZONE_MAX) y = 0;
	
	//Directional input -> Overrides stick
	if (Wii.wiiUProControllerConnected) {
		if (Wii.getButtonPress(LEFT))  x = -dSpeed[speed];
		if (Wii.getButtonPress(RIGHT)) x = +dSpeed[speed];
		if (Wii.getButtonPress(UP))    y = -dSpeed[speed];
		if (Wii.getButtonPress(DOWN))  y = +dSpeed[speed];
	}
	else if (Wii.wiimoteConnected) {
		//Assume WiiMote is held sideways
		if (Wii.getButtonPress(UP))   x = -dSpeed[speed];
		if (Wii.getButtonPress(DOWN)) x = +dSpeed[speed];
		if (Wii.getButtonPress(RIGHT))y = -dSpeed[speed];
		if (Wii.getButtonPress(LEFT)) y = +dSpeed[speed];
	}
	
	//Buttons
	bool bt_1 = Wii.getButtonPress(A) || Wii.getButtonPress(Y) || Wii.getButtonPress(ONE);
	bool bt_2 = Wii.getButtonPress(B) || Wii.getButtonPress(X) || Wii.getButtonPress(TWO);

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
void setSpeed(int delta) {
  speed = min(3, max(0, speed + delta));

  Wii.setLedOff();
  switch(speed) {
    case 3: Wii.setLedOn(LED4);
    case 2: Wii.setLedOn(LED3);
    case 1: Wii.setLedOn(LED2);
    case 0: Wii.setLedOn(LED1);
  }
}
