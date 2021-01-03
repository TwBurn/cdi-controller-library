/*
 * Example sketch for the CD-i Controller Library using a MagicNS/Switch Pro controller
 * By Jeffrey Janssen - cdi@nmotion.nl
 */

#include <SPI.h>
#include "MagicNS.h"
#include "CdiController.h"

#define PIN_RTS 2
#define PIN_RXD 3 

#define DEADZONE_AVG 128
#define DEADZONE_MAX (DEADZONE_AVG + 16)
#define DEADZONE_MIN (DEADZONE_AVG - 16)

USB Usb;
MagicNS swPro(&Usb);
CdiController Cdi(PIN_RTS, PIN_RXD, MANEUVER);

//For update limiting.
uint32_t nextUpdateTime;
uint8_t updateInterval = 20; //~50Hz

void setup() {
	Serial.begin(115200);
	Usb.Init();
	Cdi.Init();
	Serial.println("Magic NS to CD-i Started");
}

byte speed = 0;
int dSpeed[] = {4, 8, 16, 32};
int aSpeed[] = {8, 16, 32, 64};

bool allowSpeed = true;

void loop() { 
	Usb.Task();
	Cdi.Task();

	//Check if we can update
	if((int32_t)((uint32_t)millis() - nextUpdateTime) >= 0L) {
		nextUpdateTime = (uint32_t)millis() + updateInterval;
    
		//Alter Speed
		if      (swPro.inputData.ZR || swPro.inputData.R) setSpeed(+1);
		else if (swPro.inputData.ZL || swPro.inputData.L) setSpeed(-1);
		else allowSpeed = true;  
		
		//Analog Stick
		byte lhx = swPro.inputData.LeftHatX;
		byte lhy = swPro.inputData.LeftHatY;
		
		int x = map(lhx, 0, 255, -aSpeed[speed], +aSpeed[speed]);
		int y = map(lhy, 0, 255, -aSpeed[speed], +aSpeed[speed]);
		
		if (lhx > DEADZONE_MIN && lhx < DEADZONE_MAX) x = 0;
		if (lhy > DEADZONE_MIN && lhy < DEADZONE_MAX) y = 0;
		
		//Directional input -> Overrides stick
		if (swPro.inputData.LEFT)  x = -dSpeed[speed];
		if (swPro.inputData.RIGHT) x = +dSpeed[speed];
		if (swPro.inputData.UP)    y = -dSpeed[speed];
		if (swPro.inputData.DOWN)  y = +dSpeed[speed];
		
		//Buttons
		bool bt_1 = (swPro.inputData.A + swPro.inputData.Y) != 0;
		bool bt_2 = (swPro.inputData.B + swPro.inputData.X) != 0;
		
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
}

void setSpeed(int delta) {
	if (allowSpeed) {
		speed = min(3, max(0, speed + delta));
		allowSpeed = false;
	}
}
