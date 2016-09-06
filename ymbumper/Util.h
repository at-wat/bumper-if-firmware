/** ***************************************************************************
 * Description       : Common functions for USB-SPI bridge
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

#ifndef _UTIL_H_
#define _UTIL_H_


/** ************** Includes ************** **/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "Descriptors.h"

#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>


/** ************** Defines ************** **/
#ifndef _INLINE_
#define _INLINE_ static inline __attribute__((always_inline))
#endif

#define	SPD_DELAY		1
#define SPD_WAIT		0

#define GET_USB_BLOCKING	true
#define GET_USB_NONBLOCKING	false


/** ************** Function Prototypes ************** **/

/** Gets a byte from USB downstream FIFO buffer
 * If GET_USB_BLOCKING is set, the function will spinloop, waiting for a byte to appear in the buffer
 * If GET_USB_NONBLOCKING is set, the function will return immediately, returning a byte if available
 * In both cases the function will return a bool indicating if a byte was retrieved
 * \param byte : pointer to variable that will store the retrieved byte
 * \param blocking : GET_USB_BLOCKING or GET_USB_NONBLOCKING
 * \return result : true when a byte is returned, false otherwise
 */
bool getUSBByte(uint8_t * byte, bool blocking);

/** Send a byte to the USB upstream FIFO buffer
 * The function will return a bool indicating if a byte was sent
 * If this function is called when the buffer is full, the buffer is cleared (sent to host). The function
 * will then write the new byte into the buffer when it is ready.
 * \param byte : byte to send
 * \return result : true when a byte is sent, false otherwise
 */
bool sendUSBByte(uint8_t byte);

bool flushSendStream(void);


#endif
