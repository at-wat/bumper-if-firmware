/** ***************************************************************************
 * Description       : Firmware for ymbumper
 * Compiler          : GCC
 *
 * Copyright (C) Dean Camera, 2009-2010
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

#ifndef _YMBUMPER_H_
#define _YMBUMPER_H_

/** ************** Includes ************** **/
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/power.h>
#include <util/delay.h>


#include "Descriptors.h"
#include "Util.h"
/*
#include "../Common/Board/LEDs.h"
#include "../Common/Board/Buttons.h"
#include "../Common/Board/GPIO.h"
#include "../Common/Terminal/TerminalDriver.h"
*/
#include <LUFA/Version.h>
#include <LUFA/Drivers/USB/USB.h>

#include <LUFA/Drivers/Peripheral/ADC.h>


/** ************** Macros ************** **/
#ifndef _INLINE_
#define _INLINE_ static inline __attribute__((always_inline))
#endif


	/** CDC Class specific request to get the current virtual serial port configuration settings. */
	#define REQ_GetLineEncoding          0x21
	
	/** CDC Class specific request to set the current virtual serial port configuration settings. */
	#define REQ_SetLineEncoding          0x20
	
	/** CDC Class specific request to set the current virtual serial port handshake line states. */
	#define REQ_SetControlLineState      0x22
	
	/** Version major of the CDC bootloader. */
	#define BOOTLOADER_VERSION_MAJOR     0x01
	
	/** Version minor of the CDC bootloader. */
	#define BOOTLOADER_VERSION_MINOR     0x00
	
	/** Hardware version major of the CDC bootloader. */
	#define BOOTLOADER_HWVERSION_MAJOR   0x01
	
	/** Hardware version minor of the CDC bootloader. */
	#define BOOTLOADER_HWVERSION_MINOR   0x00
	
	/** Version major of the application section. */
	#define APPLICATION_VERSION_MAJOR   0x01
	
	/** Version minor of the application section. */
	#define APPLICATION_VERSION_MINOR   0x00
	
	/** 8 character bootloader firmware identifier reported to the host when requested */
	#define SOFTWARE_IDENTIFIER          "Bumper"


/** ************** Type Defines ************** **/
/** Type define for the virtual serial port line encoding settings, for storing the current USART configuration as set by the host via a class specific request. **/
typedef struct
{
	uint32_t BaudRateBPS; /** Baud rate of the virtual serial port, in bits per second **/
	uint8_t  CharFormat; /** Character format of the virtual serial port, a value from the CDCDevice_CDC_LineCodingFormats_t enum **/
	uint8_t  ParityType; /** Parity setting of the virtual serial port, a value from the CDCDevice_LineCodingParity_t enum **/
	uint8_t  DataBits; /** Bits of data per character of the virtual serial port **/
} CDC_Line_Coding_t;


/** ************** Enums ************** **/
/** Enum for the possible line encoding formats of a virtual serial port. */
enum CDCDevice_CDC_LineCodingFormats_t
{
	OneStopBit          = 0, /**< Each frame contains one stop bit */
	OneAndAHalfStopBits = 1, /**< Each frame contains one and a half stop bits */
	TwoStopBits         = 2, /**< Each frame contains two stop bits */
};

/** Enum for the possible line encoding parity settings of a virtual serial port. */
enum CDCDevice_LineCodingParity_t
{
	Parity_None         = 0, /**< No parity bit mode on each frame */
	Parity_Odd          = 1, /**< Odd parity bit mode on each frame */
	Parity_Even         = 2, /**< Even parity bit mode on each frame */
	Parity_Mark         = 3, /**< Mark parity bit mode on each frame */
	Parity_Space        = 4, /**< Space parity bit mode on each frame */
};


/** ************** Function Prototypes ************** **/
void LED_Button_Demo(void);
void PWM_Demo(void);
void Bridge_Demo(void);
void Credits(void);
void Demo_Not_Ready(uint8_t menuIndex);
uint8_t task_button_debounce(void);
void SetupHardware(void);

/* USB Functions */
void EVENT_USB_Device_Connect(void);
void EVENT_USB_Device_Disconnect(void);
void EVENT_USB_Device_Suspend(void);
void EVENT_USB_Device_Wake(void);
void EVENT_USB_Device_Reset(void);
void EVENT_USB_Device_ConfigurationChanged(void);
void EVENT_USB_Device_UnhandledControlRequest(void);

enum{
	YMBUMPER_MODE_STANDBY = 0,
	YMBUMPER_MODE_MD,
	YMBUMPER_MODE_TM,
	YMBUMPER_MODE_RAW = 0x10
};
#define YMBUMPER_MODE_COMMENT { "Standby Mode", "Measuring by Sensitive Mode", "" }

int ScipVV( unsigned char *command, unsigned long *arg );
int ScipPP( unsigned char *command, unsigned long *arg );
int ScipII( unsigned char *command, unsigned long *arg );
int ScipMeasure( unsigned char *command, unsigned long *arg );
int ScipGet( unsigned char *command, unsigned long *arg );
int ScipQT( unsigned char *command, unsigned long *arg );
int ScipTM( unsigned char *command, unsigned long *arg );
int ScipReset( unsigned char *command, unsigned long *arg );
int ScipSS( unsigned char *command, unsigned long *arg );
int ScipBM( unsigned char *command, unsigned long *arg );
int ScipUnImplemented( unsigned char *command, unsigned long *arg );
int ScipRW( unsigned char *command, unsigned long *arg );
int SetSerialNo( unsigned char *command, unsigned long *arg );
int SetThresh( unsigned char *command, unsigned long *arg );
int SetDist( unsigned char *command, unsigned long *arg );
int SetRes( unsigned char *command, unsigned long *arg );
int SetFrt( unsigned char *command, unsigned long *arg );

int main( void );




#endif
