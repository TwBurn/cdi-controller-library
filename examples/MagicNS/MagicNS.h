/*
 * Copyright (C) 2021 Jeffrey Janssen - cdi@nmotion.nl
 * 
 * This software may be distributed and modified under the terms of the GNU
 * General Public License version 2 (GPL2) as published by the Free Software
 * Foundation and appearing in the file GPL2.TXT included in the packaging of
 * this file. Please note that GPL2 Section 2[b] requires that all works based
 * on this software must also be made publicly available under the terms of
 * the GPL2 ("Copyleft").
 */

#ifndef __MagicNS_h__
#define __MagicNS_h__

#include <hiduniversal.h>

#define MAGICNS_VID 0x0079
#define MAGICNS_PID 0x18D2

struct MagicNSData {
	uint8_t X: 1;
	uint8_t A: 1;
	uint8_t B: 1;
	uint8_t Y: 1;
	uint8_t L: 1;
	uint8_t R: 1;
	uint8_t ZL: 1;
	uint8_t ZR: 1;
	
	uint8_t MIN: 1;
	uint8_t PLUS: 1;
	uint8_t SL: 1;
	uint8_t SR: 1;
	uint8_t HOME: 1;
	uint8_t dummy_0: 3;
	
	uint8_t dummy_1;
	
	uint8_t LeftHatX;
	uint8_t LeftHatY;
	uint8_t RightHatX;
	uint8_t RightHatY;
	uint8_t RIGHT;
	uint8_t LEFT;
	uint8_t UP;
	uint8_t DOWN;
} __attribute__((packed));

class MagicNS : public HIDUniversal {
public:
	MagicNS(USB *p) : HIDUniversal(p) {};
	bool connected() {
		return HIDUniversal::isReady() && HIDUniversal::VID == MAGICNS_VID && HIDUniversal::PID == MAGICNS_PID;
	};
	
	int getVID() { return HIDUniversal::VID; }
	int getPID() { return HIDUniversal::PID; }

	byte data[27];
	
	MagicNSData inputData;

private:
	void ParseHIDData(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf); // Called by the HIDUniversal library
	uint8_t OnInitSuccessful() { // Called by the HIDUniversal library on success
		if (HIDUniversal::VID != MAGICNS_VID || HIDUniversal::PID != MAGICNS_PID) // Make sure the right device is actually connected
	    	return 1;
	    return 0;
	};
};

#endif
