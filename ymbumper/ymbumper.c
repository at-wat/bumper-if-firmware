/** ***************************************************************************
 * Description       : Firmware for ymbumper
 * Compiler          : GCC
 *
 * Copyright (C) Atsushi Watanabe, 2010
 *
 * The authors of the library and template of the code:
 * Copyright (C) 2009-2010 Justin Mattair
 * Copyright (C) Dean Camera, 2009-2010
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


#include <stdio.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/eeprom.h>

#include "ymbumper.h"
#include "scip2.h"
#include "texts.h"


CDC_Line_Coding_t LineCoding = {
	.BaudRateBPS = 115200,
	.CharFormat  = OneStopBit,
	.ParityType  = Parity_None,
	.DataBits    = 8
};

#define COM_NUM		28
const Scip2CommandType scip2[ COM_NUM + 1 ] =
{
	{ "VV", { 0 },					ScipVV },
	{ "PP", { 0 },					ScipPP },
	{ "II", { 0 },					ScipII },
	{ "MD", { 4, 4, 2, 1, 2, 0 },	ScipMeasure },
	{ "MS", { 4, 4, 2, 1, 2, 0 },	ScipMeasure },
	{ "GD", { 4, 4, 2, 0 },			ScipGet },
	{ "GS", { 4, 4, 2, 0 },			ScipGet },
	{ "MR", { 4, 4, 2, 1, 2, 0 },	ScipMeasure },
	{ "ME", { 4, 4, 2, 1, 2, 0 },	ScipMeasure },
	{ "MI", { 4, 4, 2, 1, 2, 0 },	ScipMeasure },
	{ "GR", { 4, 4, 2, 0 },			ScipGet },
	{ "GE", { 4, 4, 2, 0 },			ScipGet },
	{ "GI", { 4, 4, 2, 0 },			ScipGet },
	{ "BM", { 0 },					ScipBM },
	{ "QT", { 0 },					ScipQT },
	{ "TM", { 1, 0 },				ScipTM },
	{ "RS", { 0 },					ScipReset },
	{ "RT", { 0 },					ScipReset },
	{ "RB", { 0 },					ScipReset },
	{ "SS", { 6, 0 },				ScipSS },
	{ "HS", { 0 },					ScipUnImplemented },
	{ "DB", { 2, 0 },				ScipUnImplemented },
	{ "$RW",{ 2, 0 },				ScipRW },
	{ "$SSN",{ 0 },					SetSerialNo },
	{ "$STH",{ 3 },					SetThresh },
	{ "$SDI",{ 4 },					SetDist },
	{ "$SRE",{ 3 },					SetRes },
	{ "$SFR",{ 3 },					SetFrt },
	{ "\0", { 0 },					NULL }
};

#define SERIALNO_LEN	16
static int8_t serialno_eep[SERIALNO_LEN] EEMEM = { "000000\0" };
static uint8_t thresh_eep[1] EEMEM = { 16 };
static int8_t res_eep[1] EEMEM = { -1 };
static uint8_t dist_eep[1] EEMEM = { 10 };
static int8_t frt_eep[1] EEMEM = { -1 };
int8_t serialno[SERIALNO_LEN];
uint8_t thresh;
int8_t res;
int8_t frt;
uint16_t dist;
uint8_t amax;
int8_t array_offset;

uint16_t Raw[8];
int16_t Data[8];
uint32_t Offset[8];
uint32_t Distance[8];
uint8_t iScan;
uint8_t iScanPrev;
uint8_t iScanRef;
uint8_t MsCnt;
uint32_t Timestamp;
uint8_t Mode;
uint8_t RawMode;

uint16_t MDStart;
uint16_t MDEnd;
uint16_t MDEnc;
uint16_t INTEnc;
uint8_t cMDEnc;
uint16_t MDStep;
uint16_t iMDStep;
uint16_t MDCull;
uint16_t MDNum;
uint16_t iMDNum;

uint8_t board_version;


const uint8_t adc_ch[8] = { ADC_CHANNEL0, ADC_CHANNEL1, ADC_CHANNEL4, ADC_CHANNEL5, 
	ADC_CHANNEL6, ADC_CHANNEL7, ADC_CHANNEL9, ADC_CHANNEL8 };

int ProcessDistance( uint32_t *out, uint32_t *data, uint16_t start, uint16_t end, uint16_t cull )
{
	uint16_t i;
	uint16_t ncull;
	uint32_t min;
	uint16_t nout;

	if( cull == 0 ) cull = 1;
	ncull = 0;
	min = 0x7FFFFFFF;
	nout = 0;
	for( i = start; i <= end; i ++ ){
		ncull ++;
		if( min > data[i] ){
			min = data[i];
		}
		if( ncull >= cull ){
			out[nout] = min;
			nout ++;
			ncull = 0;
			min = 0x7FFFFFFF;
		}
	}
	return nout;
}


int ScipVV( uint8_t *command, uint32_t *arg )
{
	int8_t serial[32];
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"00" );
	sendScipResponseF( VV_VEND );
	sendScipResponseF( VV_PROD );
	sendScipResponseF( VV_FIRM );
	sendScipResponseF( VV_PROT );
	sprintf( (char*)serial, VV_SERI_FMT, serialno );
	sendScipResponse( (uint8_t *)serial );
	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int ScipPP( uint8_t *command, uint32_t *arg )
{
	int8_t serial[32];
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"00" );
	sendScipResponseF( PP_MODL );
	sendScipResponseF( PP_DMIN );
	sprintf( (char*)serial, PP_DMAX_FMT, dist + 1 );
	sendScipResponse( (uint8_t *)serial );
	sprintf( (char*)serial, PP_ARES_FMT, res );
	sendScipResponse( (uint8_t *)serial );
	sendScipResponseF( PP_AMIN );
	sprintf( (char*)serial, PP_AMAX_FMT, amax );
	sendScipResponse( (uint8_t *)serial );
	sprintf( (char*)serial, PP_AFRT_FMT, frt );
	sendScipResponse( (uint8_t *)serial );
	sendScipResponseF( PP_SCAN );
	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int ScipII( uint8_t *command, uint32_t *arg )
{
	uint8_t time[16];
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"00" );
	sendScipResponseF( PP_MODL );
	sprintf( (char*)time, TIME_FMT, (unsigned int)Timestamp );
	sendScipResponse( time );
	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int ScipMeasure( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	if( arg[0] < 0 || arg[1] > amax ){
		sendScipResponse( (uint8_t *)"04" );
		sendUSBByte( '\n' );
		flushSendStream();
		return 1;
	}
	if( arg[0] > arg[1] ){
		sendScipResponse( (uint8_t *)"05" );
		sendUSBByte( '\n' );
		flushSendStream();
		return 1;
	}
	sendScipResponse( (uint8_t *)"00" );
	sendUSBByte( '\n' );
	flushSendStream();
	if( command[1] == (uint8_t)'S' ){
		MDEnc = 2;
	}else{
		MDEnc = 3;
	}
	cMDEnc = command[1];
	if( command[1] == (uint8_t)'R' ){
		INTEnc = 2;
	}else if( command[1] == (uint8_t)'E' ){
		INTEnc = 3;
	}else if( command[1] == (uint8_t)'I' ){
		INTEnc = 4;
	}else{
		INTEnc = 0;
	}

	MDStart = arg[0];
	MDEnd   = arg[1];
	MDCull  = arg[2];
	MDStep  = arg[3];
	MDNum   = arg[4];
	iMDNum  = 0;
	iMDStep = MDStep;
	
	Mode = YMBUMPER_MODE_MD;
	
	return 1;
}

int ScipRW( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	if( !(0 <= arg[0] && arg[0] <= 1) ){
		sendScipResponse( (uint8_t *)"0E" );
		sendUSBByte( '\n' );
		flushSendStream();
		return 1;
	}
	Mode = YMBUMPER_MODE_RAW;
	RawMode = arg[0];
	
	return 1;
}

int ScipGet( uint8_t *command, uint32_t *arg )
{
	int len;
	uint32_t out[6];
	
	sendScipEchoBack( command );
	if( arg[0] < 0 || arg[1] > amax ){
		sendScipResponse( (uint8_t *)"04" );
		sendUSBByte( '\n' );
		flushSendStream();
		return 1;
	}
	if( arg[0] > arg[1] ){
		sendScipResponse( (uint8_t *)"05" );
		sendUSBByte( '\n' );
		flushSendStream();
		return 1;
	}
	sendScipResponse( (uint8_t *)"00" );
	sendScipData( &Timestamp, 1, 4 );
	len = ProcessDistance( out, Distance, arg[0], arg[1], arg[2] );
	switch( command[1] )
	{
	case (uint8_t)'S':
		sendScipData( Distance + array_offset, len, 2 );
		break;
	case (uint8_t)'D':
		sendScipData( Distance + array_offset, len, 3 );
		break;
	case (uint8_t)'R':
		sendScipDataInt( Distance + array_offset, Raw + array_offset, len, 3, 2 );
		break;
	case (uint8_t)'E':
		sendScipDataInt( Distance + array_offset, Raw + array_offset, len, 3, 3 );
		break;
	case (uint8_t)'I':
		sendScipDataInt( Distance + array_offset, Raw + array_offset, len, 3, 4 );
		break;
	default:
		break;
	}

	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int ScipBM( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"00" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int ScipQT( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"00" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	Mode = YMBUMPER_MODE_STANDBY;
	
	return 1;
}

int ScipTM( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	if( Mode == YMBUMPER_MODE_TM ){
		if( arg[0] == 0 ){
			// 既に時計合わせモード
			sendScipResponse( (uint8_t *)"02" );
		}else if( arg[0] == 1 ){
			// 時刻問い合わせ
			sendScipData( &Timestamp, 1, 4 );
		}else if( arg[0] == 2 ){
			// 時計合わせモード終了
			sendScipResponse( (uint8_t *)"00" );
			Mode = YMBUMPER_MODE_STANDBY;
		}else{
			sendScipResponse( (uint8_t *)"05" );
		}
	}else{
		if( arg[0] == 0 ){
			// 時計合わせモード
			sendScipResponse( (uint8_t *)"00" );
			Mode = YMBUMPER_MODE_TM;
		}else if( arg[0] == 1 ){
			// 時刻問い合わせモードでない
			sendScipResponse( (uint8_t *)"04" );
		}else if( arg[0] == 2 ){
			// 既に終了している
			sendScipResponse( (uint8_t *)"03" );
		}else{
			sendScipResponse( (uint8_t *)"05" );
		}
	}
	sendUSBByte( '\n' );
	flushSendStream();
	return 1;
}

int ScipReset( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"00" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	if( command[1] == 'B' ) goto *0;
	
	if( command[1] == 'S' ){
		char i;
		Timestamp = 0;
		for( i = 0; i < 8; i++ ){
			Offset[i] = 0;
			Data[i] = 0;
			Raw[i] = 0;
			Distance[i] = 0;
		}
	}
	Mode = YMBUMPER_MODE_STANDBY;
	
	return 1;
}

int ScipSS( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	// RS-232C未搭載エラー
	sendScipResponse( (uint8_t *)"04" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int ScipUnImplemented( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"03" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	return 1;
}

int SetSerialNo( uint8_t *command, uint32_t *arg )
{
	uint8_t i;
	
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"03" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	if( command[ 4 ] == '\"' )
	{
		for( i = 0; i < SERIALNO_LEN - 1; i ++ )
		{
			eeprom_busy_wait();
			eeprom_write_byte( (uint8_t*)&serialno_eep[i], (uint8_t)command[i+5] );
			if( command[i+5] == '\"' || command[i+5] == '\r' || command[i+5] == '\n' ) break;
		}
		eeprom_busy_wait();
		eeprom_write_byte( (uint8_t*)&serialno_eep[i], 0 );
	}
	
	return 1;
}
int SetThresh( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"03" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	thresh = arg[0];
	eeprom_busy_wait();
	eeprom_write_byte( (uint8_t*)&thresh_eep, thresh );
	
	return 1;
}
int SetRes( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"03" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	res = arg[0];
	eeprom_busy_wait();
	eeprom_write_byte( (uint8_t*)&res_eep, res );
	
	return 1;
}
int SetFrt( uint8_t *command, uint32_t *arg )
{
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"03" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	frt = arg[0];
	eeprom_busy_wait();
	eeprom_write_byte( (uint8_t*)&frt_eep, frt );
	
	return 1;
}
int SetDist( uint8_t *command, uint32_t *arg )
{
	uint8_t i;
	
	sendScipEchoBack( command );
	sendScipResponse( (uint8_t *)"03" );
	sendUSBByte( '\n' );
	flushSendStream();
	
	dist = arg[0];
	for( i = 0; i < 2; i ++ ){
		eeprom_busy_wait();
		eeprom_write_byte( (uint8_t*)&((uint8_t*)&dist_eep)[i], ((uint8_t*)&dist)[i] );
	}
	
	return 1;
}

int main( void )
{
	uint8_t i;
	uint8_t read_buffer[ 64 ];
	uint32_t w_read_buffer;

	/* This only sets up hardware not already setup in the bootloader */
	SetupHardware();

	if( PIND & 0x01 )
	{
		board_version = 16;
	}
	else
	{
		board_version = 17;
	}
	switch( board_version )
	{
	case 16:
		amax = 7;
		array_offset = 0;
		break;
	case 17:
		amax = 3;
		array_offset = 4;
		break;
	default:
		amax = 0;
		break;
	}

	iScan = 0;
	iScanPrev = 0;
	Timestamp = 0;
	Mode = YMBUMPER_MODE_STANDBY;
	
	for( i = 0; i < SERIALNO_LEN; i ++ ){
		eeprom_busy_wait();
		serialno[i] = eeprom_read_byte( (uint8_t*)&serialno_eep[i] );
	}
	eeprom_busy_wait();
	thresh = eeprom_read_byte( (uint8_t*)&thresh_eep );
	eeprom_busy_wait();
	res = eeprom_read_byte( (uint8_t*)&res_eep );
	eeprom_busy_wait();
	frt = eeprom_read_byte( (uint8_t*)&frt_eep );
	for( i = 0; i < 2; i ++ ){
		eeprom_busy_wait();
		((uint8_t*)&dist)[i] = eeprom_read_byte( (uint8_t*)&((uint8_t*)&dist_eep)[i] );
	}

	if( frt < 0 ){
		switch( board_version )
		{
		case 16:
			frt = 4;
			res = 8;
			break;
		case 17:
			frt = 2;
			res = 4;
			break;
		default:
			frt = 0;
			res = 0;
			break;
		}
	}

	for( i = 0; i < 8; i++ ){
		Offset[i] = 0;
		Data[i] = 0;
		Raw[i] = 0;
		Distance[i] = 0;
	}

	w_read_buffer = 0;
	MsCnt = 0;

	ADC_StartReading(adc_ch[iScan], ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED);
	for (;;) {
		uint8_t readbyte = 0;
		
		if( getUSBByte( &readbyte, GET_USB_NONBLOCKING ) ){
			if( readbyte == '\r' || readbyte == '\n' ){
				read_buffer[w_read_buffer] = 0;
				scip2CommandAnalysis( scip2, read_buffer );
				w_read_buffer = 0;
			}else if( readbyte == 0 ){
			}else{
				read_buffer[w_read_buffer] = readbyte;
				w_read_buffer ++;
				if( w_read_buffer > 63 ) w_read_buffer = 63;
			}
		}
		if( Mode == YMBUMPER_MODE_MD ){
			uint8_t cmd_state[16];
			
			if(MsCnt >= 10)
			{
				if( iMDStep >= MDStep ){
					uint32_t len;
					uint32_t out[6];
					
					iMDStep = 0;
					sprintf( (char*)cmd_state, MD_FMT, 
								(int)cMDEnc, (int)MDStart, (int)MDEnd, 
								(int)MDCull, (int)MDStep, (int)(MDNum - iMDNum) );
					if( MDNum != 0 ) iMDNum ++;
					if( iMDNum >= MDNum && MDNum != 0) Mode = YMBUMPER_MODE_STANDBY;

					sendScipEchoBack( cmd_state );
					sendScipResponse( (uint8_t *)"99" );
					sendScipData( &Timestamp, 1, 4 );
					len = ProcessDistance( out, Distance, MDStart, MDEnd, MDCull );
					if( INTEnc == 0 ){
						sendScipData( Distance + array_offset, len, MDEnc );
					}else{
						sendScipDataInt( Distance + array_offset, Raw + array_offset, len, MDEnc, INTEnc );
					}
					sendUSBByte( '\n' );
					flushSendStream();
				}
				else
				{
					iMDStep ++;
				}
				MsCnt = 0;
			}
			MsCnt ++;
		}else if( Mode == YMBUMPER_MODE_RAW ){
			uint8_t cmd_state[64];
			switch( RawMode ){
			case 0:
				sprintf( (char*)cmd_state, "%04d %04d %04d %04d  %04d %04d %04d %04d\n", 
					(int)Raw[0], (int)Raw[1], (int)Raw[2], (int)Raw[3],
					(int)Raw[4], (int)Raw[5], (int)Raw[6], (int)Raw[7]);
				break;
			case 1:
				sprintf( (char*)cmd_state, "%04d %04d %04d %04d  %04d %04d %04d %04d\n", 
					(int)Data[0]+1000, (int)Data[1]+1000, (int)Data[2]+1000, (int)Data[3]+1000,
					(int)Data[4]+1000, (int)Data[5]+1000, (int)Data[6]+1000, (int)Data[7]+1000);
				break;
			default:
				cmd_state[0] = 0;
				break;
			}
			sendUSBString( cmd_state );
			flushSendStream();
		}
	}
}

ISR(TIMER0_COMPA_vect, ISR_BLOCK)
{
	Timestamp ++;
	Timestamp &= 0xFFFFFF;
	MsCnt ++;
}

ISR(ADC_vect, ISR_BLOCK)
{
	if( iScanRef & 1 )
	{
		Raw[iScan] = ADC_GetResult();
		if( Offset[iScan] == 0 ) Offset[iScan] = (uint32_t)Raw[iScan] * 0x1000;
		Offset[iScan] = Offset[iScan] - (Offset[iScan] / 0x1000) + Raw[iScan];
		Data[iScan] = Raw[iScan] - Offset[iScan] / 0x1000;

		if(Data[iScan] > thresh)
		{
			Distance[iScan] = dist;
		}
		else
		{
			Distance[iScan] = 0;
		}
		
		iScanPrev = iScan;
		iScan ++;
		if(iScan == 8) iScan = 0;
		ADMUX = (adc_ch[iScan] & 0x1F) | ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED;
		ADCSRB &= ~(1 << MUX5);
		if( adc_ch[iScan] & 0x20 ) ADCSRB |= (1 << MUX5);
	}
	else
	{
		ADMUX = (ADC_1100MV_BANDGAP & 0x1F) | ADC_REFERENCE_AVCC | ADC_RIGHT_ADJUSTED;
		ADCSRB &= ~(1 << MUX5);
		if( ADC_1100MV_BANDGAP & 0x20 ) ADCSRB |= (1 << MUX5);
	}
	iScanRef ++;
}


/** Configures all hardware */
void SetupHardware(void)
{
	/* Turn on pullups of unused inputs. Pins can be left floating, however, in order to
	minimize power consumption, pullups must be enabled. This is because when pullups are disabled,
	the input goes into a high-impedance state (tristated). The trace connected to the pin can act
	as an antenna. This antenna will easily pick up EMI. If it is strong enough to overcome the hysteresis of
	the built-in schmitt trigger, the pin will toggle rapidly. This toggling will consume more power.
	The default when the chip resets is to leave the inputs floating. This default is intended to avoid
	hardware conflicts. You may need to adjust these settings if having pullups enabled would
	adversely affect circuitry you have connected to the AT90 header pins. */
	
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	
	MCUCR &= ~(1 << PUD);
	MCUCR |= (1 << JTD);
	PORTD = 0x01;
	
	/* Set cpu prescaler based on crystal frequency */
#if (F_CLOCK == F_CPU)
	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (F_CLOCK == 16000000 && F_CPU == 8000000)
	/* set cpu prescaler to 2 */
	clock_prescale_set(clock_div_2);
#endif
	
	/* set sleep mode */
	set_sleep_mode(SLEEP_MODE_IDLE);
	
	
	/* initialize timer0 */
	TCCR0A = 0b00000010;
	TCCR0B = 0b00000011;
	TIMSK0 = (1 << OCIE0A);	// enable timer counter 1 interrupt on compare
	OCR0A  = 250;
	
	
	ADC_Init(ADC_FREE_RUNNING | ADC_PRESCALE_128 | (1 << ADIE));
	
	ADC_SetupChannel(0);
	ADC_SetupChannel(1);
	ADC_SetupChannel(4);
	ADC_SetupChannel(5);
	ADC_SetupChannel(6);
	ADC_SetupChannel(7);
	ADC_SetupChannel(9);
	ADC_SetupChannel(8);
	
	USB_Init();
}


/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	set_sleep_mode(SLEEP_MODE_IDLE);
//	LED_Status_On();
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	sei();
//	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_mode();
}

void EVENT_USB_Device_Suspend (void)
{
	sei();
//	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	//sleep_mode();
}

void EVENT_USB_Device_WakeUp (void)
{
	set_sleep_mode(SLEEP_MODE_IDLE);
}

void EVENT_USB_Device_Reset (void)	// TODO
{
	
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	/* Indicate USB connected and ready */

		/* Setup CDC Notification, Rx and Tx Endpoints */
		if (!(Endpoint_ConfigureEndpoint(CDC_NOTIFICATION_EPNUM, EP_TYPE_INTERRUPT,
		      ENDPOINT_DIR_IN, CDC_NOTIFICATION_EPSIZE, ENDPOINT_BANK_SINGLE)))
		{
		}
	
		if (!(Endpoint_ConfigureEndpoint(CDC_TX_EPNUM, EP_TYPE_BULK,
		      ENDPOINT_DIR_IN, CDC_TXRX_EPSIZE, ENDPOINT_BANK_DOUBLE)))
		{
		}

		if (!(Endpoint_ConfigureEndpoint(CDC_RX_EPNUM, EP_TYPE_BULK,
		      ENDPOINT_DIR_OUT, CDC_TXRX_EPSIZE, ENDPOINT_BANK_DOUBLE)))
		{
		}
}

/** Event handler for the USB_UnhandledControlRequest event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the CDC control commands,
 *  which are all issued via the control endpoint), so that they can be handled appropriately for the application.
 */
void EVENT_USB_Device_UnhandledControlRequest(void)
{
		uint8_t* LineCodingData = (uint8_t*)&LineCoding;
		
		/* Process CDC specific control requests */
		switch (USB_ControlRequest.bRequest)
		{
			case REQ_GetLineEncoding:
				if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					Endpoint_ClearSETUP();

					for (uint8_t i = 0; i < sizeof(LineCoding); i++)
						Endpoint_Write_Byte(*(LineCodingData++));	
				
					Endpoint_ClearIN();
				
					Endpoint_ClearStatusStage();
				}
				break;
				
			case REQ_SetLineEncoding:
				if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					Endpoint_ClearSETUP();

					while (!(Endpoint_IsOUTReceived()))
					{
						if (USB_DeviceState == DEVICE_STATE_Unattached)
							return;
					}
			
					for (uint8_t i = 0; i < sizeof(LineCoding); i++)
						*(LineCodingData++) = Endpoint_Read_Byte();

					Endpoint_ClearOUT();

					Endpoint_ClearStatusStage();
				}
				break;
				
			case REQ_SetControlLineState:
				if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
				{
					Endpoint_ClearSETUP();
				
					Endpoint_ClearStatusStage();
				}
				break;
		}
}

