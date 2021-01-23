/*
 * Example sketch for the CD-i Controller Library using a WiiMote or Wii U Pro controller
 * By Jeffrey Janssen - cdi@nmotion.nl
 */
 
#include <Wii.h>
#include <usbhub.h>
#include <CdiController.h>


#define JOY_MIN 1000
#define JOY_MAX 3000

#define DEADZONE_AVG 2000
#define DEADZONE_MAX (DEADZONE_AVG + 200)
#define DEADZONE_MIN (DEADZONE_AVG - 200)

USB Usb;
BTD Btd(&Usb);

WII *Wii[2];
CdiController *Cdi[2];
bool oldControllerState[2];

void setup() {
	Serial.begin(115200);
	Wii[0] = new WII(&Btd, PAIR);
	Wii[0]->attachOnInit(onInit);
	Wii[1] = new WII(&Btd, PAIR);
	Wii[1]->attachOnInit(onInit);

	Cdi[0] = new CdiController(2, 3, MANEUVER, 0);
	Cdi[1] = new CdiController(5, 6, MANEUVER, 1);
	
	Usb.Init();
	
	Serial.println("Wii TwoPlayer Started");
	
}

//Joystick speed selection
byte speeds[] = {0, 0};
int dSpeed[] = {4, 8, 16, 32};  //Digital (D-pad)
int aSpeed[] = {8, 16, 32, 64}; //Analog (Stick)

void loop() {
	Usb.Task();

	for (byte i = 0; i < 1; i++) {
		if (!(Wii[i]->wiimoteConnected || Wii[i]->wiiUProControllerConnected)) continue;
		
		Cdi[i]->Task();
	
		//Disconnect on HOME
		if (Wii[i]->getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
			Serial.println("Disconnecting Controller");
			Wii[i]->disconnect();
		}
		
		//Alter Speed
		if (Wii[i]->getButtonClick(PLUS))  setSpeed(i, +1);
		if (Wii[i]->getButtonClick(MINUS)) setSpeed(i, -1);

		int speed = speeds[i];
		
		//Analog Stick
		
		int lhx = Wii[i]->getAnalogHat(LeftHatX);
		int lhy = Wii[i]->getAnalogHat(LeftHatY);
		//int rhx = Wii[i]->getAnalogHat(RightHatX);
		//int rhy = Wii[i]->getAnalogHat(RightHatY);
		
		lhx = constrain(lhx, JOY_MIN, JOY_MAX);
		lhy = constrain(lhy, JOY_MIN, JOY_MAX);
		//rhx = constrain(rhx, JOY_MIN, JOY_MAX);
		//rhy = constrain(rhy, JOY_MIN, JOY_MAX);
		
		int x = map(lhx, JOY_MIN, JOY_MAX, -aSpeed[speed], +aSpeed[speed]);
		int y = map(lhy, JOY_MAX, JOY_MIN, -aSpeed[speed], +aSpeed[speed]); //Inverted
		
		if (lhx > DEADZONE_MIN && lhx < DEADZONE_MAX) x = 0;
		if (lhy > DEADZONE_MIN && lhy < DEADZONE_MAX) y = 0;
		
		//Directional input -> Overrides stick
		if (Wii[i]->wiiUProControllerConnected) {
			if (Wii[i]->getButtonPress(LEFT))  x = -dSpeed[speed];
			if (Wii[i]->getButtonPress(RIGHT)) x = +dSpeed[speed];
			if (Wii[i]->getButtonPress(UP))    y = -dSpeed[speed];
			if (Wii[i]->getButtonPress(DOWN))  y = +dSpeed[speed];
		}
		else if (Wii[i]->wiimoteConnected) {
			//Assume WiiMote is held sideways
			if (Wii[i]->getButtonPress(UP))   x = -dSpeed[speed];
			if (Wii[i]->getButtonPress(DOWN)) x = +dSpeed[speed];
			if (Wii[i]->getButtonPress(RIGHT))y = -dSpeed[speed];
			if (Wii[i]->getButtonPress(LEFT)) y = +dSpeed[speed];
		}
		
		//Buttons
		bool bt_1 = Wii[i]->getButtonPress(A) || Wii[i]->getButtonPress(Y) || Wii[i]->getButtonPress(ONE);
		bool bt_2 = Wii[i]->getButtonPress(B) || Wii[i]->getButtonPress(X) || Wii[i]->getButtonPress(TWO);
	
		bool updated = Cdi[i]->JoyInput((byte)x, (byte)y, bt_1, bt_2);
	}
}
void setSpeed(byte player, int delta) {
  speeds[player] = min(3, max(0, speeds[player] + delta));
}

void onInit() {
	for (uint8_t i = 0; i < 2; i++) {
		if ((Wii[i]->wiimoteConnected || Wii[i]->wiiUProControllerConnected) && !oldControllerState[i]) {
			oldControllerState[i] = true;
			Serial.print("Wii controller ");
			Serial.print(i + i);
			Serial.print(" connected");
			Serial.println();
			Wii[i]->setLedOff();
			Wii[i]->setLedOn((LEDEnum)(i + 1)); // Cast directly to LEDEnum - see: "controllerEnums.h"
		}
	}
}
