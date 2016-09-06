/** ***************************************************************************
 * Description       : GPIO definitions and functions for JM-DB-U2
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

#ifndef __GPIO_JM_DB_U2_H__
#define __GPIO_JM_DB_U2_H__

/** Includes: **/
#include <avr/io.h>


/** Defines **/
#ifndef _INLINE_
#define _INLINE_ static inline __attribute__((always_inline))
#endif

#define AUDIO		(1 << 0)
#define SS		(1 << 0)
#define SCLK		(1 << 1)
#define MOSI		(1 << 2)
#define MISO		(1 << 3)


/** Inline Functions: **/
	/* PWM Audio pin */
	_INLINE_ void Audio_Init_Output(void) {		// output low
		PORTD &=  ~AUDIO;
		DDRD  |=  AUDIO;
	}
	_INLINE_ void Audio_Init_Input(void) {		// input without pullup
		DDRD  &=  ~AUDIO;
		PORTD &=  ~AUDIO;
	}
	_INLINE_ void Audio_High(void) {
		PORTD |= AUDIO;
	}
	_INLINE_ void Audio_Low(void) {
		PORTD &= ~AUDIO;
	}
	_INLINE_ void Audio_Toggle(void) {
		PORTD = (PORTD ^ (AUDIO));
	}
	
	_INLINE_ uint8_t Audio_GetStatus(void)
	{
		return (PIND & AUDIO);
	}

	
	/* SS */
	_INLINE_ void SS_Init(void) {	// default output high
		PORTB |=  SS;
		DDRB  |=  SS;
	}
	_INLINE_ void SS_High(void) {
		PORTB |= SS;
	}
	_INLINE_ void SS_Low(void) {
		PORTB &= ~SS;
	}
	_INLINE_ void SS_Toggle(void) {
		PORTB = (PORTB ^ (SS));
	}
	
	_INLINE_ uint8_t SS_GetStatus(void)
	{
		return (PINB & SS);
	}
	
	/* SCLK */
	_INLINE_ void SCLK_Init(void) {		// default output high
		PORTB |=  SCLK;
		DDRB  |=  SCLK;
	}
	_INLINE_ void SCLK_High(void) {
		PORTB |= SCLK;
	}
	_INLINE_ void SCLK_Low(void) {
		PORTB &= ~SCLK;
	}
	_INLINE_ void SCLK_Toggle(void) {
		PORTB = (PORTB ^ (SCLK));
	}
	
	_INLINE_ uint8_t SCLK_GetStatus(void)
	{
		return (PINB & SCLK);
	}
	
	/* MOSI */
	_INLINE_ void MOSI_Init(void) {		// default output high
		PORTB |=  MOSI;
		DDRB  |=  MOSI;
	}
	_INLINE_ void MOSI_High(void) {
		PORTB |= MOSI;
	}
	_INLINE_ void MOSI_Low(void) {
		PORTB &= ~MOSI;
	}
	_INLINE_ void MOSI_Toggle(void) {
		PORTB = (PORTB ^ (MOSI));
	}
	
	_INLINE_ uint8_t MOSI_GetStatus(void)
	{
		return (PINB & MOSI);
	}
	
	/* MISO */
	_INLINE_ void MISO_Init(void) {		// default input without pullup
		PORTB &=  ~MISO;
		DDRB  &=  ~MISO;
	}
	
	_INLINE_ uint8_t MISO_GetStatus(void)
	{
		return (PINB & MISO);
	}
	
	
#endif
