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

#ifndef _UTILITIES_H_
#define _UTILITIES_H_


/** ************** Includes ************** **/
#include <avr/eeprom.h>


/** ************** Defines ************** **/
#ifndef _INLINE_
#define _INLINE_ static inline __attribute__((always_inline))
#endif


/** ************** Function Prototypes ************** **/
uint16_t decodeHex(void);
void encodeHex(uint16_t word);
uint8_t getEEPROMByte (uint8_t * address);
void putEEPROMByte (uint8_t * address, uint8_t byte);


#endif
