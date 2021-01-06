/*
 * Example sketch for the CD-i Controller Library using a USB mouse and/or keyboard to emulate a Relative Coordinate Device
 * By Jeffrey Janssen - cdi@nmotion.nl
 */
 
#include <CdiController.h>

#include <hidboot.h>
#include <usbhub.h>
#include <SPI.h>

#define PIN_RTS 2
#define PIN_RXD 3

/* Keyboard and Mouse Event Handlers */
class CdiMouseRptParser : public MouseReportParser {
  protected:
  	void OnMouseEvent(MOUSEINFO *mi);
    void OnMouseMove(MOUSEINFO *mi)        { OnMouseEvent(mi); };
    void OnLeftButtonUp(MOUSEINFO *mi)     { OnMouseEvent(mi); };
    void OnLeftButtonDown(MOUSEINFO *mi)   { OnMouseEvent(mi); };
    void OnRightButtonUp(MOUSEINFO *mi)    { OnMouseEvent(mi); };
    void OnRightButtonDown(MOUSEINFO *mi)  { OnMouseEvent(mi); };
};

class CdiKeyboardRptParser : public KeyboardReportParser {
  protected:
  	void OnKeyEvent (uint8_t key, bool pressed);
    void OnKeyDown	(uint8_t mod, uint8_t key) { OnKeyEvent(key, true); };
    void OnKeyUp	(uint8_t mod, uint8_t key) { OnKeyEvent(key, false); };
};

//Declare all devices
CdiController Cdi(PIN_RTS, PIN_RXD, RELATIVE);
USB Usb;
USBHub Hub(&Usb);

HIDBoot<USB_HID_PROTOCOL_KEYBOARD | USB_HID_PROTOCOL_MOUSE> HidComposite(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE>    HidMouse(&Usb);

//Mouse
CdiMouseRptParser CdiMouse;

void CdiMouseRptParser::OnMouseEvent(MOUSEINFO *mi) {
	//All Mouse Events are handled in the same way
	if (Cdi.JoyInput((byte)mi->dX, (byte)mi->dY, (bool)mi->bmLeftButton, (bool)mi->bmRightButton)) {
		Serial.print("Mouse: ");
		Serial.print("x=");
		Serial.print(mi->dX);
		Serial.print(" y=");
		Serial.print(mi->dY);
		Serial.print(" B1=");
		Serial.print(mi->bmLeftButton);
		Serial.print(" B2=");
		Serial.print(mi->bmRightButton);
		Serial.println();
	}
}

//Keyboard Events
CdiKeyboardRptParser CdiKeyboard;

//Keyboard speeds: 1   2   3   4   5   6   7   8   9   0
int keySpeeds[] = {1,  2,  4,  8, 12, 16, 20, 24, 28, 32};
int keySpeed = 4; //Default Speed Index -> "5"

//Keyboard variables; communication with the CD-i is done in the loop() function, as we only get events and we need to keep sending movement messages
bool keyUp;
bool keyDown;
bool keyLeft;
bool keyRight;
bool keyButton1;
bool keyButton2;

void CdiKeyboardRptParser::OnKeyEvent(uint8_t key, bool state) {
	switch(key) {
		case 0x52: keyUp      = state; break; //UP
		case 0x51: keyDown    = state; break; //DOWN
		case 0x50: keyLeft    = state; break; //LEFT
		case 0x4F: keyRight   = state; break; //RIGHT
		case 0x1D: keyButton1 = state; break; //Z
		case 0x1B: keyButton2 = state; break; //X
	}

	//Speed selection, use number keys "1" (0x1E) to "0" (0x27)
	if (state && key >= 0x1E && key <= 0x27) keySpeed = key - 0x1E;
}

void setup() {
	Serial.begin(115200);
	Usb.Init();
	Cdi.Init();
	
	delay(200);

	//Setup Keyboard and Mouse event handlers
	HidComposite.SetReportParser(0, &CdiKeyboard);
	HidComposite.SetReportParser(1, &CdiMouse);
	HidKeyboard.SetReportParser(0, &CdiKeyboard);
	HidMouse.SetReportParser(0, &CdiMouse);
	
	
	Serial.println("Keyboard/Mouse to CD-i started");
}

void loop() {
	Usb.Task();
	Cdi.Task();

	//Handle Keyboard input
	int curSpeed = keySpeeds[keySpeed];
	int keyMoveX = keyRight * curSpeed - keyLeft * curSpeed;
	int keyMoveY = keyDown  * curSpeed - keyUp   * curSpeed;
		
	if (Cdi.JoyInput((byte)keyMoveX, (byte)keyMoveY, keyButton1, keyButton2)) {
		Serial.print("Keyboard: ");
		Serial.print("x=");
		Serial.print(keyMoveX);
		Serial.print(" y=");
		Serial.print(keyMoveY);
		Serial.print(" B1=");
		Serial.print(keyButton1);
		Serial.print(" B2=");
		Serial.print(keyButton2);
		Serial.println();
	}
}
