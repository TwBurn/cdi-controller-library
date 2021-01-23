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
 
#include "CdiController.h"

void CdiController::Init() {
	//Set RTS Pin to INPUT, initialise Serial Port
	pinMode(rtsPin, INPUT);
	serialPort->initialize(rxdPin);
}

void CdiController::Task() {
	//Assert RTS
	if (digitalRead(rtsPin)) {
		//If we weren't connected, make ourselves known to the CD-i player
		if (!connected) {
			delay(100); //Specification asks for a delay between first asserting RTS state and writing the mode
			serialPort->write(mode);
			connected = true;
		}
	}
	else {
		//CD-i player has been turned off, set connected to false so we can wait for it to turn on
		connected = false;
	}
}

bool CdiController::JoyInput(uint8_t x, uint8_t y, bool button_1, bool button_2) {
	//Setup the button bits
	uint8_t buttons = (button_1 << 5) | (button_2 << 4);

	//Don't transmit if we aren't connected or if nothing has changed
	if (!connected || (x == 0 && y == 0 && buttons == oldButtons) || commBusy()) return false;
	
	//Initial bitmasks
	uint8_t b0 = 0b11000000;
	uint8_t b1 = 0b10000000;
	uint8_t b2 = 0b10000000;
	
	//Set X, Y, button bits
	b0 = b0 | (x >> 6) | ((y & 0b11000000) >> 4) | buttons;
	b1 = b1 | (x & 0b00111111);
	b2 = b2 | (y & 0b00111111);
	
	//Send to CD-i player
	serialPort->write(b0);
	serialPort->write(b1);
	serialPort->write(b2);
	
	//Save old button state
	oldButtons = buttons;
	
	//Data has been sent
	return true;
}

bool CdiController::PenInput(uint16_t x, uint16_t y, bool button_1, bool button_2, bool pen_down) {
	//Don't transmit if we aren't connected
	if (!connected || commBusy()) return false;

	//Limit input to 10 bits
	x = x & 0x3FF;
	y = y & 0x3FF;
	
	//Setup the button bits
	uint8_t buttons = (button_1 << 5) | (button_2 << 4);
	
	//Initial bitmasks
	uint8_t b0 = 0b11000000;
	uint8_t b1 = 0b10000000;
	uint8_t b2 = 0b10000000;
	uint8_t b3 = 0b10000000;

	//Set X, Y, button bits
	b0 = b0 | (x >> 6) | buttons;
	b1 = b1 | (y >> 6) | (pen_down << 6);
	b2 = b2 | (x & 0b00111111);
	b3 = b3 | (y & 0b00111111);
		
	//Send data to CD-i player
	serialPort->write(b0);
	serialPort->write(b1);
	serialPort->write(b2);
	serialPort->write(b3);

	//Data has been sent
	return true;
}

bool CdiController::KeyInput(uint8_t key_code, uint8_t extention, bool shift, bool capslock, bool alt, bool ctrl) {
	if (!connected || commBusy()) return false;
		
	//Limit Extention bits
	extention = extention & 0x03;
		
	//Setup the button bits
	uint8_t buttons = (shift << 3) | (capslock << 4) | (alt << 5) | (ctrl << 6);
	
	//Initial bitmasks
	uint8_t b0 = 0b10000000;
	uint8_t b1 = 0b00000000;
	
	//Set bits
	b0 = b0 | buttons | (extention << 2) | (key_code >> 7);
	b1 = b1 | (key_code & 0b01111111);
	
	//Send data to CD-i player
	serialPort->write(b0);
	serialPort->write(b1);
	
	//Data has been sent
	return true;
}

bool CdiController::KeyPress(uint8_t key_code, bool shift, bool capslock, bool alt, bool ctrl) {
	return KeyInput(key_code, 0x00, shift, capslock, alt, ctrl);
}
bool CdiController::KeyRelease(bool shift, bool capslock, bool alt, bool ctrl) {
	return KeyInput(0x00, 0x01, shift, capslock, alt, ctrl);
}
	
bool CdiController::KeyReset() {
	return KeyInput(0x00, 0x01, false, false, false, false);
}
