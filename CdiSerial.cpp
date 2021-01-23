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
 
#include "CdiSerial.h"

CdiSerial CdiPlayers[2];

void CdiSerial::initialize(uint8_t _transmitPin) {
	transmitPin = _transmitPin;
	
	//Set Transmit Pin to OUTPUT, setup timer
	pinMode(transmitPin, OUTPUT);

	Timer1.initialize(830);
	Timer1.attachInterrupt(timerCallback);
	Timer1.start();
}

void CdiSerial::serialWrite() {
	if (transmitPin == 0xFF) return;
	digitalWrite(transmitPin, (data & 1) ? LOW : HIGH);
}
void CdiSerial::serialNext() {
	if (transmitPin == 0xFF) return;
	if (data == 1) { //Since the stop bit is high, when curData == 1 the full byte has been sent.
		if (readIndex != writeIndex) {
			//Move to and prepare next byte
			data = (buffer[readIndex] << 1) | 0x200; //Add start and stop bit
			readIndex = (readIndex + 1) % sizeof(buffer);
		}
	}
	else {
		data = data >> 1;
	}
}

static void CdiSerial::timerCallback() {
	CdiPlayers[0].serialWrite();
	CdiPlayers[1].serialWrite();
	CdiPlayers[0].serialNext();
	CdiPlayers[1].serialNext();
}
