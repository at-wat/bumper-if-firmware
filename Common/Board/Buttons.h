/** ***************************************************************************
 * Description       : Buttons definitions and functions for JM-DB-U2
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

#ifndef __BUTTONS_JM_DB_U2_H__
#define __BUTTONS_JM_DB_U2_H__

/** Includes: **/
#include <avr/io.h>


/** Defines **/
#ifndef _INLINE_
#define _INLINE_ static inline __attribute__((always_inline))
#endif

#define HWB_BUTTON      (1 << 7)


/** Inline Functions: **/
/* HWB Button */
_INLINE_ void HWB_Button_Init(void)	// input with pullup
{
	DDRD  &= ~HWB_BUTTON;
	PORTD |=  HWB_BUTTON;
}

_INLINE_ uint8_t HWB_Button_GetStatus(void)
{
	return ((PIND & HWB_BUTTON) ^ HWB_BUTTON);
}


#endif
