/** ***************************************************************************
 * Description       : LEDs definitions and functions for JM-DB-U2
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
 
#ifndef __LEDS_JM_DB_U2_H__
#define __LEDS_JM_DB_U2_H__

/** Includes: **/
#include <avr/io.h>


/** Defines **/
#ifndef _INLINE_
#define _INLINE_ static inline __attribute__((always_inline))
#endif

/* LEDs on Port D */
#define LED_STATUS_PIN		(1 << 4)

/** Inline Functions: **/
/* Status LED (green) */
_INLINE_ void LED_Status_Init(void)	// default off (low)
{
	PORTD &= ~LED_STATUS_PIN;
	DDRD  |=  LED_STATUS_PIN;
}

_INLINE_ void LED_Status_On(void)
{
	PORTD |= LED_STATUS_PIN;
}

_INLINE_ void LED_Status_Off(void)
{
	PORTD &= ~LED_STATUS_PIN;
}

_INLINE_ void LED_Status_Toggle(void)
{
	PORTD = (PORTD ^ (LED_STATUS_PIN));
}


#endif
