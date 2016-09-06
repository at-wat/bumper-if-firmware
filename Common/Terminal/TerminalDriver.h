/** ***************************************************************************
 * Description       : Terminal emulation interface
 * Compiler          : GCC
 *
 * Copyright (C) 2009-2010 Justin Mattair
 * Copyright (C) 2003 Atmel Corporation
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

#ifndef _TERMINALDRIVER_H
#define _TERMINALDRIVER_H


/** ************** Includes ************** **/
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdbool.h>

#include "TerminalEvents.h"
#include "../Utilities.h"


/** ************** Defines ************** **/
/* cursor modes */
#define MODE_NONE         '0'
#define MODE_BOLD         '1'
#define MODE_DIM          '2'
#define MODE_UNDERLINE    '4'
#define MODE_BLINK        '5'
#define MODE_REVERSED     '7'
#define MODE_CONCEALED    '8'

/* Text colors */
#define COL_FOREGROUND    '3'
#define COL_BACKGROUND    '4'

#define COL_BLACK         '0'
#define COL_RED           '1'
#define COL_GREEN         '2'
#define COL_YELLOW        '3'
#define COL_BLUE          '4'
#define COL_MAGENTA       '5'
#define COL_CYAN          '6'
#define COL_WHITE         '7'


/* keys / selections */
#define	TERM_CONTROL	0
#define TERM_CONTROL_CODES_START	0x20
#define TERM_GET_RECEIVED_CHAR	0
#define TERM_GET_TASK_EVENT	1
#define MOVE_UP		'A'
#define MOVE_DOWN	'B'
#define MOVE_RIGHT	'C'
#define MOVE_LEFT	'D'
#define	KEY_ENTER	13
#define	KEY_BACKSPACE	8

#define SELECTION_NULL		0
#define SELECTION_BACK		1
#define SELECTION_CANCEL	1
#define SELECTION_OK		2
#define SELECTION_MIN		1
#define SELECTION_BACK_MIN	2

/* dimensions */
#define COLUMN_SPACING 4
#define WIDTH 80
#define HEIGHT 24
#define HALF_WIDTH (WIDTH / 2)
#define HALF_HEIGHT (HEIGHT / 2)


/* These are used mainly by sendString */
#define SEND_STRING_SOURCE_FLASH	0x00
#define SEND_STRING_SOURCE_EEPROM	0x01
#define SEND_STRING_SOURCE_RAM		0x02
#define SEND_STRING_SOURCE_MASK		0x03

#define SEND_STRING_ACTION_SEND		0x00
#define SEND_STRING_ACTION_SILENT	0x04
#define SEND_STRING_ACTION_MASK		0x04

#define SEND_STRING_COUNT_CHAR		0x00
#define SEND_STRING_COUNT_RECORD	0x08
#define SEND_STRING_COUNT_MASK		0x08

#define	SEND_STRING_DEST_TERMINAL	0x00
#define	SEND_STRING_DEST_BUFFER		0x10
#define SEND_STRING_DEST_MASK		0x10


/* These are used for the mode argument of Term_Handle_Menu to specify the type of menu/dialog/edit box */
/* I may eventually use separate functions or macros that call Term_Handle_Menu() with valid combinations of the following */
#define	MENU_LOCATION_FLASH	0x00	// menu/header located in FLASH (default)
#define	MENU_LOCATION_EEPROM	0x01	// menu/header located in EEPROM
#define MENU_LOCATION_MASK	0x01	// mask for menu/header location

#define	MENU_DUAL_COLUMN	0x02	// Dual-column (second column created in RAM buffer)
#define	MENU_NONSELECT_OK	0x04	// Items not selectable, displays OK at bottom
#define	MENU_SCROLLING		0x08	// Scrolling (default is wrapping)
#define	MENU_BACK_CANCEL	0x10	// list 'back' as 1st item (cancel at bottom if MENU_NONSELECT_OK set)

#define	MENU_EDIT_BYTE		0x20	// edit box for one byte in hex
#define	MENU_EDIT_WORD		0x40	// edit box for two bytes (word) in hex
#define	MENU_EDIT_DWORD		0x60	// edit box for four bytes (dword) in hex
#define	MENU_EDIT_MASK		0x60	// mask for both edit bits

#define	MENU_IMMEDIATE_RETURN	0x80	// return immediately after drawing (let caller handle input)

/* menu argument is a pointer to the first-column char array used for the menu. MENU_LOCATION_FLASH
 * and MENU_LOCATION_EEPROM above specify the location in the mode argument. If a second column is
 * used, it is always located in RAM. If a RAM buffer is used for the first column, a null pointer
 * should be passed as the menu argument. Note that the second column is disabled in this case. */
#define	MENU_LOCATION_RAM	0	// used in place of menu pointer argument to signal RAM as location


/** ************** Macros ************** **/
/* Send escape sequence start */
#define SENDESC            \
    Term_Send( 27 );       \
    Term_Send( '[' );


/** ************** Function Prototypes ************** **/
void Term_Init (void);
void Term_Park_Cursor(void);
void Term_Reset_Screen_Text(void);
void Term_Send_Value_as_Digits (uint8_t value);
void Term_Send_Value_as_Hex (uint8_t value);
void Term_Send (uint8_t data);
uint16_t Term_Get_Sequence (void);
uint8_t sendString(const char * ptr, uint8_t mode, uint8_t index);
uint8_t Term_Send_FlashStr (const char * str);
uint8_t Term_Send_RAMStr (uint8_t index);
void Term_Erase_Screen (void);
void Term_Set_Display_Attribute_Mode (uint8_t mode);
//void Term_Set_Display_Colour (uint8_t fg_bg, uint8_t colour);
void Term_Set_Cursor_Position (uint8_t row, uint8_t column);
void Term_Draw_Frame (uint8_t top, uint8_t left, uint8_t height, uint8_t width);
uint8_t Term_Draw_Menu (const char * menu, const char * headerMenu, uint8_t headerIndex, uint8_t mode, uint8_t selectPos, uint8_t xPosition);
uint8_t Term_Handle_Menu (const char * menu, const char * headerMenu, uint8_t headerIndex, uint8_t mode, uint8_t selectPos);
uint8_t editByte (const char * menu, uint8_t menuIndex, uint8_t * byte);
void encodeHexByteToBuffer (uint8_t byte, uint8_t newline);
void RepeatCharacter (const char character, uint8_t repeat);


#endif
