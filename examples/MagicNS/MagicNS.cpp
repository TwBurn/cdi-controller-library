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

#include "MagicNS.h"

void MagicNS::ParseHIDData(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
	if (HIDUniversal::VID != MAGICNS_VID || HIDUniversal::PID != MAGICNS_PID) // Make sure the right device is actually connected
		return;
	
	memcpy(&inputData, buf, min(len, MFK_CASTUINT8T sizeof(inputData)));
}
