/** ***************************************************************************
 * Description       : Common functions
 * Compiler          : GCC
 *
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

#include "Utilities.h"

/* Global Variables */
uint8_t hexChar[4];


uint16_t decodeHex(void)	/* convert up to 4 ascii hex characters to integer word */
{
	uint16_t ret = 0;
	uint8_t value;
	uint8_t shift = 12;
	
	for (uint8_t i = 0; i < 4; i++) {
		if (hexChar[i] >= '0' && hexChar[i] <= '9') {
			value = hexChar[i] - '0';
		} else {	// should be A-F
			value = hexChar[i] - 55;
		}
		ret += (value * (1 << shift));
		shift -= 4;
	}
	for (uint8_t i = 0; i < 4; i++) {	// set all characters to '0' for next call
		hexChar[i] = '0';
	}
	
	return ret;
}

void encodeHex(uint16_t word)	/* convert integer word to four ascii hex characters */
{
	uint8_t digit;
	uint16_t number = 4096;
	
	for (uint8_t i = 0; i < 4; i++) {	// set all characters to '0'
		hexChar[i] = '0';
	}
	
	for (uint8_t i = 0; i < 3; i++) {
		digit = '0';
		while (word >= number) {
			if (digit == '9') {
				digit = 'A';
			} else {
				digit++;
			}
			word -= number;
		}
		hexChar[i] = digit;
		number = (number >> 4); 
	}
	
	if (word >= 10) {
		digit = 'A' + (word - 10);
	} else {
		digit = '0' + word;
	}
	hexChar[3] = digit;
}

uint8_t getEEPROMByte (uint8_t * address)
{
	eeprom_busy_wait();
	return (eeprom_read_byte (address));
}

void putEEPROMByte (uint8_t * address, uint8_t byte)
{
	eeprom_busy_wait();
	eeprom_write_byte (address, byte);
}
