/** ***************************************************************************
 * Description       : Terminal emulation interface Events
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

#ifndef _TERMINALEVENTS_H
#define _TERMINALEVENTS_H


/** ************** Function Prototypes ************** **/
#if !defined(INCLUDE_FROM_EVENTS_C)
	void Event_Term_Send(uint8_t data);
	void Event_Term_Flush(void);
	uint8_t Event_Term_Get(uint8_t * byte);
	uint8_t Event_Term_IsInput(void);
	void Event_Term_Delay(void);
	void Event_PrintHeader(void);
#endif

#if defined(INCLUDE_FROM_EVENTS_C)
	void Terminal_Event_Stub(void) __attribute__ ((const));
	void Event_Term_Send(void) __attribute__ ((weak, alias("Terminal_Event_Stub")));
	void Event_Term_Flush(void) __attribute__ ((weak, alias("Terminal_Event_Stub")));
	void Event_Term_Get(void) __attribute__ ((weak, alias("Terminal_Event_Stub")));
	void Event_Term_IsInput(void) __attribute__ ((weak, alias("Terminal_Event_Stub")));
	void Event_Term_Delay(void) __attribute__ ((weak, alias("Terminal_Event_Stub")));
	void Event_PrintHeader(void) __attribute__ ((weak, alias("Terminal_Event_Stub")));
#endif


#endif
