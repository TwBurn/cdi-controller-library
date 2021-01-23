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
 
#ifndef __CdiSerial_h__
#define __CdiSerial_h__

#include "Arduino.h"
#include <TimerOne.h>


class CdiSerial {
public:
	//Constructor
	CdiSerial() {}

	void initialize(uint8_t _transmitPin);
	void write(uint8_t data) {
		buffer[writeIndex] = data;
		writeIndex = (writeIndex + 1) % sizeof(buffer);
	}
	void stop() { Timer1.stop();}
	bool isBusy() {
		return (writeIndex != readIndex);
	}
protected:
	uint8_t transmitPin = 0xFF; //Pin number on the Arduino for the RXD (Data Transfer) pin (pin#2 on the CD-i player)
	
	
private:
	uint8_t buffer[8];
	uint8_t writeIndex = 0;
	uint8_t readIndex = 0;

	uint16_t data = 1;
	
	void serialWrite();
	void serialNext();
	
	static void timerCallback();
};

extern CdiSerial CdiPlayers[2];
#endif
