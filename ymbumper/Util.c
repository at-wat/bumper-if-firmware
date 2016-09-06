/** ***************************************************************************
 * Description       : Bridge functions for USB-SPI bridge on AT90
 * Compiler          : GCC
 *
 * Modified by Atsushi WATANABE 2010
 * Copyright (C) 2009-2010 Justin Mattair
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 *************************************************************************** **/

#include "Util.h"

/* Global Variables */
volatile uint8_t spd_timed_out = false;


bool getUSBByte(uint8_t * byte, bool blocking)
{
	/* Select the OUT endpoint so that the next data byte can be read */
	Endpoint_SelectEndpoint(CDC_RX_EPNUM);
	
	/* If OUT endpoint empty, clear it and wait for the next packet from the host */
	while (!(Endpoint_IsReadWriteAllowed())) {
		Endpoint_ClearOUT();
		while (!(Endpoint_IsOUTReceived())) {
			if (Endpoint_WaitUntilReady()) return false;
			if (!blocking) break;
		}
		if (!blocking) break;
	}
	
	/* Fetch the next byte from the OUT endpoint if available */
	if (Endpoint_IsReadWriteAllowed()) {
		*byte = Endpoint_Read_Byte();
		return true;
	} else {
		return false;
	}
}

bool sendUSBByte(uint8_t byte)
{
	/* Select the IN endpoint so that the next data byte can be written */
	Endpoint_SelectEndpoint(CDC_TX_EPNUM);
	
	/* If IN endpoint full, clear it and wait until ready for the next packet to the host */
	if (!(Endpoint_IsReadWriteAllowed())) {
		if (!flushSendStream()) return false;
	}
	
	/* Write the next byte to the OUT endpoint */
	Endpoint_Write_Byte(byte);
	
	return true;
}

bool flushSendStream (void)
{
	uint8_t ret = 0;
	bool IsFull = (Endpoint_BytesInEndpoint() == CDC_TXRX_EPSIZE);
	Endpoint_ClearIN();
	if (IsFull) {
		ret = Endpoint_WaitUntilReady();
		/* Send an empty packet to ensure that the host does not buffer data sent to it */
		Endpoint_ClearIN();
	}
	ret |= Endpoint_WaitUntilReady();
	
	if (ret) {
		return false;
	} else {
		return true;
	}
}
