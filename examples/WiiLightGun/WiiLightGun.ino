/*
 * Example sketch for the CD-i Controller Library using a WiiMote as Lightgun (Absolute Coordinate Device)
 * By Jeffrey Janssen - cdi@nmotion.nl
 */
 
#include <Wii.h>
#include <usbhub.h>
#include <CdiController.h>

#ifndef WIICAMERA // Used to check if WIICAMERA is defined
#error "Please set ENABLE_WII_IR_CAMERA to 1 in settings.h"
#endif

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

CdiController Cdi(PIN_RTS, PIN_RXD, ABSOLUTE);

//For update limiting.
uint32_t nextUpdateTime;
uint8_t updateInterval = 20; //~50Hz

void setup() {
	Serial.begin(115200);
	Usb.Init();
	Cdi.Init();
	Serial.println("WiiMote Lightgun to CD-i started");
}

int x;
int y;
bool tracking = false;
bool active = false;

void loop() {
	Usb.Task();
	Cdi.Task();

	//Check if we can update
	if(Wii.wiimoteConnected && (int32_t)((uint32_t)millis() - nextUpdateTime) >= 0L) {
		nextUpdateTime = (uint32_t)millis() + updateInterval;
		
		if (Wii.getButtonClick(ONE)) {
        	Wii.IRinitialize(); // Run the initialisation sequence
        	active = true;
        	Serial.println("IR Active");
		}

		if (Wii.getButtonClick(TWO)) {
        	Wii.IRinitialize(); // Run the initialisation sequence
        	active = false;
        	Serial.println("IR Inactive");
		}
      
      	if (Wii.getButtonClick(MINUS) || Wii.getButtonClick(PLUS)) {
	        if (!Wii.isIRCameraEnabled())
        	  	Serial.println("Enable IR camera first");
        	else {
        		if (Wii.getButtonPress(MINUS)) { // getButtonClick will only return true once
        			tracking = false;
        			Wii.setLedOff();
        			Serial.println("Tracking Off");
          		}
          		else {
          			tracking = true;
          			Wii.setLedOn(LED4);
    				Wii.setLedOn(LED3);
    				Wii.setLedOn(LED2);
    				Wii.setLedOn(LED1);
          			Serial.println("Tracking On");
          		}
	        }
    	}

    	//Disconnect on HOME
		if (Wii.getButtonClick(HOME)) { // You can use getButtonPress to see if the button is held down
			Serial.println("Disconnecting Controller");
			Wii.disconnect();
			tracking = false;
			active = false;
		}

		//Track
		if (active) {
			if (Wii.getIRx1() != 0x3FF || Wii.getIRy1() != 0x3FF || Wii.getIRs1() != 0x0F) { // Only print if the IR camera is actually seeing something
				x = 1023 - Wii.getIRx1(); //Inverse Axis
				y = map(Wii.getIRy1(), 0, 768, 0, 1023); //Increase range
			}
			
			if (tracking) {
				Serial.print(F("x1: "));
				Serial.print(Wii.getIRx1());
				Serial.print(F("\ty1: "));
				Serial.print(Wii.getIRy1());
				Serial.print(F("\ts1:"));
				Serial.print(Wii.getIRs1());
				Serial.println();
			}
			//Buttons
			bool bt_1 = Wii.getButtonPress(A);
			bool bt_2 = Wii.getButtonPress(B);
	
			bool updated = Cdi.PenInput(x, y, bt_1, bt_2, false);
	
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
}
