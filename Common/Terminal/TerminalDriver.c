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

/* This terminal driver grew from something much simpler. It badly needs to be rewritten after adding many features. However, it does only occupy around 2.5KB of code space and very little RAM (depends on buffer) while supporting single or dual-column menus, edit boxes, dialogs with OK (and optionally cancel), and other misc features. It will eventually grow into something better organised, with many more features. My intent is to end up with something that can be conditionally compiled into a small footprint, with the most common features, yet be much simpler than something like the various curses libraries available. For now, if you really want to use it, you can look at how my other code currently uses this to figure it out, as I won't document until the rewrite. The original use was to have labels in the first column (which are stored in FLASH or EEPROM), and optionally a second column using the RAM buffer for dynamic data (like configuration settings). */

/* TODO: The terminal driver will undergo changes but have mostly the same funtionality.
The file selector and slider (along with other widgets) will go into a mid-level
layer that calls the terminal library */

#include "TerminalDriver.h"

/* Global variables */
extern uint8_t hexChar[];
uint8_t buffer[TERMINAL_BUFFER_SIZE];	// TERMINAL_BUFFER_SIZE defined in makefile
uint8_t * bufferPtr;
uint8_t ctlChar;
uint8_t printPass = 1;
uint8_t escapeDecodeState;

/* Message Text */
const char BeginText[] PROGMEM = "Press any key";
const char EditHex[] PROGMEM = "New value in hex:";
const char BackText[] PROGMEM = "Back";
const char OKText[] PROGMEM = " OK ";
const char CancelText[] PROGMEM = " CANCEL ";


/** Clear screen, print header, print BeginText[], wait for keypress **/
void Term_Init (void)
{
	uint8_t wait_for_keypress = 1;
	uint8_t dummy;
	
	while (wait_for_keypress) {
		Term_Erase_Screen();
		Event_PrintHeader();
		Term_Set_Cursor_Position(HALF_HEIGHT, (HALF_WIDTH - (sizeof(BeginText) / 2)));
		Term_Send_FlashStr (BeginText);
		Term_Park_Cursor();
		Event_Term_Flush();
		if (Event_Term_IsInput()) {
			wait_for_keypress = 0;
			Event_Term_Get(&dummy);
		} else {
			Event_Term_Delay();
		}
	}
}

/** Put cursor in parking location, for use with menus **/
void Term_Park_Cursor(void)	// TODO macro
{
	Term_Set_Cursor_Position(6, 1);
}

/** Erase screen, print header, and position cursor at (7,1) for text output **/
void Term_Reset_Screen_Text (void)
{
	Term_Erase_Screen();
	Event_PrintHeader();
	Term_Set_Cursor_Position(7, 1);
}

/** Convert byte to 3 ASCII digits and send **/
void Term_Send_Value_as_Digits (uint8_t value)
{
	uint8_t digit;
	
	digit = '0';
	while (value >= 100) {	// Still larger than 100 ?
		digit++;	// Increment first digit
		value -= 100;
	}
	Term_Send (digit);	// Send first digit
	
	digit = '0';
	while (value >= 10) {	// Still larger than 10 ?
		digit++;	// Increment second digit
		value -= 10;
	}
	Term_Send (digit);	// Send second digit
	
	Term_Send ('0' + value);	// Send third digit
}

/**  Convert byte to 2 Hex digits and send **/
void Term_Send_Value_as_Hex (uint8_t value)
{
	encodeHex(value);
	Term_Send (hexChar[2]);	// Send first digit
	Term_Send (hexChar[3]);	// Send second digit
}

/**  Transmit one byte **/
void Term_Send (uint8_t data)	// TODO macro?
{
	if (printPass) {
		Event_Term_Send(data);
	}
}

/** Decode incoming ESC sequence **/
uint16_t Term_Get_Sequence (void)
{
	uint8_t val;
	uint16_t ret;
	
	ret = Event_Term_Get(&val);
	
	if (ret == TERM_GET_TASK_EVENT) {
		ret = 0;
		ret = val << 8;	// Put code in upper byte
	} else {
		switch (escapeDecodeState) {
			case 0:
				if (val != 27) {		// Received ESC ?
					ret = val;		// If not, return char
					break;
				}
				escapeDecodeState = 1;
				ret = Event_Term_Get(&val);
			case 1:
				if (ret == TERM_GET_TASK_EVENT) {
					ret = 0;
					ret = val << 8;	// Put code in upper byte
					break;
				}
				if (val != '[') {	// Received '[' ?
					ret = val;	// If not, return char
					escapeDecodeState = 0;
					break;
				}
				escapeDecodeState = 2;
				ret = Event_Term_Get(&val);
			case 2:
				if (ret == TERM_GET_TASK_EVENT) {
					ret = 0;
					ret = val << 8;	// Put code in upper byte
					break;
				}
				if (val == 'A' || val == 'B' || val == 'C' || val == 'D') {  // Arrow keys
					ret = val << 8;	// Put code in upper byte
				} else {
					ret = val;	// If not, return char
				}
				escapeDecodeState = 0;
				break;
		}
	}
	return ret;
}

/** Send string **/
uint8_t sendString(const char * ptr, uint8_t mode, uint8_t index)
{
	uint8_t byte = 1;
	uint8_t prevByte;
	uint8_t count = 0;
	uint8_t numRecords = 0;
	
	while (byte != 0) {
		prevByte = byte;
		if ((mode & SEND_STRING_SOURCE_MASK) == SEND_STRING_SOURCE_FLASH) {
			byte = pgm_read_byte(ptr++);
		} else if ((mode & SEND_STRING_SOURCE_MASK) == SEND_STRING_SOURCE_EEPROM) {
			byte = eeprom_read_byte((uint8_t *)&ptr);
			ptr++;
		} else {
			byte = *ptr++;
		}
		
		while (byte != '\n' && byte != 0) {
			if (index == numRecords) {
				if ((mode & SEND_STRING_ACTION_MASK) == SEND_STRING_ACTION_SEND) {
					if ((mode & SEND_STRING_DEST_MASK) == SEND_STRING_DEST_BUFFER) {
						*bufferPtr++ = byte;
					} else {
						Term_Send (byte);
					}
				}
				count++;
			}
			prevByte = byte;
			if ((mode & SEND_STRING_SOURCE_MASK) == SEND_STRING_SOURCE_FLASH) {
				byte = pgm_read_byte(ptr++);
			} else if ((mode & SEND_STRING_SOURCE_MASK) == SEND_STRING_SOURCE_EEPROM) {
				byte = eeprom_read_byte((uint8_t *)&ptr);
				ptr++;
			} else {
				byte = *ptr++;
			}
		}
		numRecords++;
	}
	
	if ((mode & SEND_STRING_DEST_MASK) == SEND_STRING_DEST_BUFFER) {
		*bufferPtr++ = '\n';
	}
	
	if ((mode & SEND_STRING_COUNT_MASK) == SEND_STRING_COUNT_RECORD) {
		if (prevByte == '\n') numRecords--;
		return numRecords;
	} else {
		return count;
	}
}

/** Send a string to from flash to terminal, return byte count **/
uint8_t Term_Send_FlashStr (const char * str)	// TODO macro?, backward compatability
{
	return sendString(str, SEND_STRING_SOURCE_FLASH , 0);
}

/** Send a string from buffer to terminal, return byte count **/
uint8_t Term_Send_RAMStr (uint8_t index)	// TODO macro?, backward compatability
{
	return sendString((const char *)buffer, SEND_STRING_SOURCE_RAM, index);
	
/*	uint8_t count = 0;
	uint8_t i = 0;
	
	while (index != count) {
		while (buffer[i] != '\n') {
			i++;
		}
		i++;
		count++;
	}
	count = 0;
	
	while (buffer[i] != '\n') {
		Term_Send (buffer[i]);
		i++;
		count++;
	}

	return count;
*/
}

/** Send 'clear screen' sequence **/
void Term_Erase_Screen (void)
{
	SENDESC
	Term_Send ('2');
	Term_Send ('J');
}

/** Set text mode **/
void Term_Set_Display_Attribute_Mode (uint8_t mode)
{
	SENDESC
	Term_Send (mode);
	Term_Send ('m');
}

/** Set cursor position, top-left is (1,1) **/
void Term_Set_Cursor_Position (uint8_t row, uint8_t column)
{
	SENDESC
	Term_Send_Value_as_Digits (row);	// Convert row byte
	Term_Send (';');
	Term_Send_Value_as_Digits (column);	// Convert column byte
	Term_Send ('H');
}


/** Draw frame on terminal client **/
void Term_Draw_Frame (uint8_t top, uint8_t left, uint8_t height, uint8_t width)
{	
	uint8_t i = 0;
	
	left--;
	height++;
	width += 3;
	
	/*** Draw frame ***/
	Term_Set_Cursor_Position( top, left );		// Corners first
	Term_Send( '+' );
	
	Term_Set_Cursor_Position( top, left + width );
	Term_Send( '+' );
	
	Term_Set_Cursor_Position( top + height, left );
	Term_Send( '*' );
	
	Term_Set_Cursor_Position( top + height, left + width );
	Term_Send( '*' );
	
	for( i = left + 1; i < left + width; i++ ) {		// Top and bottom edges
		Term_Set_Cursor_Position( top, i );
		Term_Send( '-' );
		
		// single line below header
		Term_Set_Cursor_Position( top + 2, i );
		Term_Send( '-' );
		
		Term_Set_Cursor_Position( top + height, i );
		Term_Send( '-' );
	}
	
	for( i = top + 1; i < top + height; i++ ) {		// Left and right edges
		Term_Set_Cursor_Position( i, left );
		Term_Send( '|' );
		
		Term_Set_Cursor_Position( i, left + width );
		Term_Send( '|' );
	}
}


/** Draw menu/selection/dialog/edit box on terminal client. Called by Term_Handle_Menu. Work in progress. This grew from something simple and now could use a rewrite **/
uint8_t Term_Draw_Menu (const char * menu, const char * headerMenu, uint8_t headerIndex, uint8_t mode, uint8_t selectPos, uint8_t xPosition)
{
	uint8_t totalHeight, i, j, k, width, jwidth, height, top, left, selectOffset, linePrinted;
	uint8_t columnTwoStart, numEditChars, padding;
	uint8_t menuLocation, headerMenuLocation;
	const char * ptr;
	const char * ptrHeader = headerMenu;
	
	width = jwidth = height = i = j = k = linePrinted = totalHeight = padding = 0;
	columnTwoStart = top = left = 1;
	
	headerMenuLocation = menuLocation = (mode & MENU_LOCATION_MASK);
	
	if (menu == 0) {	// if null pointer, RAM buffer will be used for first (and only) column
		ptr = (const char *)buffer;
		menuLocation = SEND_STRING_SOURCE_RAM;
	} else {		// otherwise the first column is from FLASH or EEPROM
		ptr = menu;
	}
	
	numEditChars = ((mode & MENU_EDIT_MASK) >> 5) * 2;
	if (numEditChars == 6) {
		numEditChars = 8;
	}
	
	/* determine height and selectOffset */
	height = sendString(ptr, (SEND_STRING_ACTION_SILENT | SEND_STRING_COUNT_RECORD | menuLocation), 0);
	
	/* header */
	selectOffset = 3;
	height += 2;
	
	if (mode & MENU_NONSELECT_OK) {		// if non-select / OK
		if (mode & MENU_BACK_CANCEL) {	// if CANCEL and OK
			i = 3;
		} else if (!(mode & MENU_IMMEDIATE_RETURN)) {
			i = 2;			// OK only
		}
		if (!numEditChars) {
			selectOffset = (height + 2);
		}
		height += i;
	} else if (mode & MENU_BACK_CANCEL) {		// back button
		height++;
	}
	totalHeight = height;
	
	/* First pass to calculate sizes, second pass actually prints */
	for (printPass = 0; printPass < 2; printPass++) {
		height = 0;
		if (menu == 0) {   // if null pointer, RAM buffer will be used for first (and only) column
			ptr = (const char *)buffer;
			menuLocation = SEND_STRING_SOURCE_RAM;
		} else {		// otherwise the first column is from FLASH or EEPROM
			ptr = menu;
		}
		ptrHeader = headerMenu;
		while (height < totalHeight) {
			height++;
			i = j = 0;
			linePrinted = 0;
			
			if (!numEditChars && height == (selectPos + selectOffset - 1)) {
				Term_Set_Display_Attribute_Mode (MODE_REVERSED);  // Reverse selected item
			} else if (numEditChars && height <= (totalHeight - 3) && height == (selectOffset + selectPos - 1)) {
				Term_Set_Display_Attribute_Mode (MODE_REVERSED);  // Reverse selected item
			} else if (numEditChars && height > (totalHeight - 2) && height == (selectOffset + selectPos)) {
				Term_Set_Display_Attribute_Mode (MODE_REVERSED);  // Reverse selected item
			} else {
				Term_Set_Display_Attribute_Mode (MODE_NONE);
			}
			
			Term_Set_Cursor_Position(top + height, left + 1);	// Start of item text
			
			if (height == 1) {	// header
				if (k > width) {
					k = 0;
				} else {
					k = ((width / 2) - (k / 2) + 1);
				}
				Term_Set_Cursor_Position (top + height, left + k); // Centered header
				i = sendString(ptrHeader, headerMenuLocation, headerIndex);
				k = i;
				linePrinted = 1;
			} else if (height == 2) {	// header
				linePrinted = 1;	// blank line
			} else if (height == 3) {
				if ((mode & MENU_BACK_CANCEL) && !(mode & MENU_NONSELECT_OK)) {
					i = Term_Send_FlashStr (BackText);	// print back option
					for (uint8_t x = 0; x < (width - i); x++) {
						Term_Send (' '); // selected text filling after columns
					}
					linePrinted = 1;	// back option after header
				}
			} else if (height == (selectOffset - 1) && (mode & MENU_NONSELECT_OK) && !numEditChars) {
				linePrinted = 1;	// blank line before cancel (OK)
			} else if (numEditChars && height == (totalHeight - 2)) {
				linePrinted = 1;	// blank line before cancel (OK)
			} else if ((height == selectOffset && (mode & MENU_NONSELECT_OK) && !numEditChars) || (numEditChars && height == (totalHeight - 1))) {
				if (mode & MENU_BACK_CANCEL) {
					i = ((width / 2) - 3);
					Term_Set_Cursor_Position (top + height, left + i);
					i = Term_Send_FlashStr (CancelText);	// print CANCEL option
					linePrinted = 1;
				} else {
					i = ((width / 2) - 1);
					Term_Set_Cursor_Position (top + height, left + i);
					i = Term_Send_FlashStr (OKText);	// print OK option
					linePrinted = 1;
				}
			} else if (height == totalHeight && (mode & MENU_BACK_CANCEL) && (mode & MENU_NONSELECT_OK)) {
				i = ((width / 2) - 1);
				Term_Set_Cursor_Position (top + height, left + i);
				i = Term_Send_FlashStr (OKText);	// print OK option
				linePrinted = 1;
			}
			
			if (!linePrinted) {
				i += sendString(ptr, menuLocation, 0);
				ptr += (i + 1);
				
				if (mode & MENU_DUAL_COLUMN) {	// if dual-column
					/* print second column */
					if (numEditChars) {
						Term_Set_Display_Attribute_Mode (MODE_NONE);
					}
					for (uint8_t x = 0; x < (columnTwoStart - i); x++) {
						Term_Send (' '); // selected text filling between columns
					}
					if (numEditChars) {
						for (uint8_t x = 0; x < numEditChars; x++) {
							if (xPosition == x && selectPos == (height - 2)) {
								Term_Set_Display_Attribute_Mode (MODE_BLINK);
							} else {
								Term_Set_Display_Attribute_Mode (MODE_NONE);
							}
							Term_Send (buffer[((height - 3) * numEditChars) + x]);
						}
						j = numEditChars;
						if (selectPos == (height - 2)) {
							Term_Set_Display_Attribute_Mode (MODE_REVERSED);
						} else {
							Term_Set_Display_Attribute_Mode (MODE_NONE);
						}
					} else {
						j = Term_Send_RAMStr (height - 3 - ((mode & MENU_BACK_CANCEL) >> 4));
					}
					padding = (width - (j + columnTwoStart));
				} else {
					padding = (width - i);
				}
				for (uint8_t x = 0; x < padding; x++) {
					Term_Send (' '); // selected text filling after columns
				}
			}
			
			/* determine maximum width */
			if (i > width) {
				width = i;
			}
			if (j > jwidth) {
				jwidth = j;
			}
		}
		
		/* set top and left to make frame auto-centered */
		top = (HALF_HEIGHT) - ((totalHeight + 2) / 2);
		if (top < 7) {
			top = 7;
		}
		if (mode & MENU_DUAL_COLUMN) {	// if dual-column
			columnTwoStart = width + COLUMN_SPACING;
			if (!printPass) {
				width += (COLUMN_SPACING + jwidth);
			}
		}
		left = (HALF_WIDTH) - ((width + 4) / 2);
	}
	
	Term_Set_Display_Attribute_Mode (MODE_NONE);
	Term_Draw_Frame (top, left, totalHeight, width);
	//Term_Set_Display_Attribute_Mode (MODE_CONCEALED);
	/* Parking location for cursor, some terminal emulators do not support a concealed cursor */
	Term_Park_Cursor();
	//Term_Set_Display_Attribute_Mode (MODE_NONE);
	if (numEditChars) {
		return (totalHeight - 3);
	} else {
		return (totalHeight - selectOffset + 1);
	}
}

/** Handle menu - mainly deals with user input **/
uint8_t Term_Handle_Menu (const char * menu, const char * headerMenu, uint8_t headerIndex, uint8_t mode, uint8_t selectPos)
{
	uint8_t height, numEditChars;
	uint8_t xPosition = 0;
	uint16_t ret;
	
	numEditChars = ((mode & MENU_EDIT_MASK) >> 5) * 2;
	if (numEditChars == 6) {
		numEditChars = 8;
	}
	
	while (1) {
		Term_Erase_Screen();
		Event_PrintHeader();
		/* Print menu text with selected item reversed */
		height = Term_Draw_Menu(menu, headerMenu, headerIndex, mode, selectPos, xPosition);
		Event_Term_Flush();
		
		if (mode & MENU_IMMEDIATE_RETURN) {	// return immediately, caller handles input
			ctlChar = TERM_CONTROL;
			return TERM_CONTROL;
		}
		
		/* Wait for user input, decode ESC sequence. Uses Event_Term_Get() */
		ret = Term_Get_Sequence();
		
		/* If user hit enter and we are not currently editing a value */
		if (ret == KEY_ENTER && !((mode & MENU_EDIT_MASK) && selectPos <= (height - 2))) {
			ctlChar = KEY_ENTER;
			return (selectPos);
		}
		
		if ((ret & 0xff) == TERM_CONTROL) {
			ctlChar = (uint8_t)(ret >> 8);
			if (ctlChar < TERM_CONTROL_CODES_START) {	// task event
				return TERM_CONTROL;
			} else if (ctlChar == MOVE_UP) {
				selectPos--;	// Move selection up
				if (selectPos < 1) {
					if (mode & MENU_SCROLLING) {	// scrolling
						return (1);
					} else {		// wrapping
						selectPos = height;
					}
				}
			} else if (ctlChar == MOVE_DOWN) {
				selectPos++;	// Move selection down, with wrap
				if (selectPos > height) {
					if (mode & MENU_SCROLLING) {	// scrolling
						return (height);
					} else {		// wrapping
						selectPos = 1;
					}
				}
			} else if (ctlChar == MOVE_LEFT) {
				if ((mode & MENU_EDIT_MASK)) {
					if (xPosition > 0) {
						xPosition--;
					}
				} else {	// caller handles (ie: slider)
					return (selectPos);
				}
			} else if (ctlChar == MOVE_RIGHT) {
				if ((mode & MENU_EDIT_MASK)) {
					if (xPosition < (numEditChars - 1)) {
						xPosition++;
					}
				} else {	// caller handles (ie: slider)
					return (selectPos);
				}
			}
		} else if ((mode & MENU_EDIT_MASK) && selectPos <= (height - 2)) {	/* editable lines */
			if ((xPosition < numEditChars) && ((ret >= '0' && ret <= '9') || (ret >= 'A' && ret <= 'F') || (ret >= 'a' && ret <= 'f'))) {
				if (ret >= 'a' && ret <= 'f') {
					ret -= 0x20;
				}
				buffer[(((selectPos - 1) * numEditChars) + xPosition)] = ret;
				if (xPosition < (numEditChars - 1)) {
					xPosition++;
				}
			} else if (ret == KEY_BACKSPACE && (xPosition > 0)) {
				xPosition--;
			}
		}
	}
}

/** edit a byte value, returns 1 if changed, this function will likely be moved **/
uint8_t editByte (const char * menu, uint8_t menuIndex, uint8_t * byte)
{
	uint8_t selection;
	
	encodeHex(*byte);
	buffer[0] = hexChar[2];
	buffer[1] = hexChar[3];
	selection = Term_Handle_Menu (EditHex, menu, menuIndex, MENU_DUAL_COLUMN | MENU_NONSELECT_OK | MENU_BACK_CANCEL | MENU_EDIT_BYTE, 1);
	if (selection == 3) {
		hexChar[2] = buffer[0];
		hexChar[3] = buffer[1];
		*byte = (uint8_t)decodeHex();
		return 1;
	}
	return 0;
}

void encodeHexByteToBuffer (uint8_t byte, uint8_t newline)
{
	encodeHex(byte);
	*bufferPtr++ = hexChar[2];
	*bufferPtr++ = hexChar[3];
	if (newline) {
		*bufferPtr++ = '\n';
	}
}

void RepeatCharacter (const char character, uint8_t repeat)
{
	for (uint8_t c = 0; c < repeat; c++) {
		Term_Send (character);
	}
}
