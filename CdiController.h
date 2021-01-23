/*
 * Copyright (C) 2021 Jeffrey Janssen - cdi@nmotion.nl
 * 
 * This software may be distributed and modified under the terms of the GNU
 * General Public License version 3 (GPL3) as published by the Free Software
 * Foundation and appearing in the file GPL-3.0.TXT included in the packaging
 * of this file. Please note that GPL3 Section 2[b] requires that all works
 * based on this software must also be made publicly available under the terms
 * of the GPL3 ("Copyleft").
 */
 
#ifndef __CdiController_h__
#define __CdiController_h__

#include "Arduino.h"
#include "CdiSerial.h"

enum CdiDevices: uint8_t {
	RELATIVE = 0b11001101, //'M' Relative Input Devices (Mouse, etc)
	MANEUVER = 0b11001010, //'J' Maneuvering Devices (Joystick, Joypad, etc)
	ABSOLUTE = 0b11010100, //'T' Absolute Coordinate Devices (Graphics Tablet, etc)
	SCREEN   = 0b11010011, //'S' Absolute Screen Devices (Touch-screens, etc)
	KEYBOARD = 0b11001011, //'K' Keyboard
	EXT_KEYB = 0b11011000  //'X' Extended Keyboard
};


class CdiController {
public:
	//Constructor
	CdiController(uint8_t _rtsPin, uint8_t _rxdPin, uint8_t _mode, uint8_t _playerNumber = 0) {
		rtsPin = _rtsPin;
		rxdPin = _rxdPin;
		mode   = _mode;

		serialPort = &CdiPlayers[_playerNumber];
		connected = false;
	}

	void Init(); //Initialisation function
	void Task(); //Task function, check for CD-i player connectivity

	//Input function for Relative and Maneuvering devices
	bool JoyInput(uint8_t x, uint8_t y, bool button_1, bool button_2);

	//Input function for Absolute Coordinate and Absolute Screen devices
	bool PenInput(uint16_t x, uint16_t y, bool button_1, bool button_2, bool pen_down);
	
	//Input function for Keyboard and Extended Keyboard
	bool KeyPress(uint8_t key_code, bool shift, bool capslock, bool alt, bool ctrl);
	bool KeyRelease(bool shift, bool capslock, bool alt, bool ctrl);
	bool KeyReset();
	bool KeyInput(uint8_t key_code, uint8_t extention, bool shift, bool capslock, bool alt, bool ctrl);

	bool IsConnected() { return connected; }
	uint8_t RTSPin()   { return rtsPin; }
	uint8_t RXDPin()   { return rxdPin; }
protected:
	bool connected; //Are we connected?
	uint8_t rtsPin; //Pin number on the Arduino for the RTS (Request To Send) pin (pin#7 on CD-i player)
	uint8_t rxdPin; //Pin number on the Arduino for the RXD (Data Transfer) pin (pin#2 on the CD-i player)
	uint8_t mode;   //Current mode
	
	inline bool commBusy() {
	return serialPort->isBusy();
	}
private:
	uint8_t oldButtons;        //Old button state
	CdiSerial *serialPort;
};

#endif
