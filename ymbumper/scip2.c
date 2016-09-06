
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/power.h>
#include <util/delay.h>

#include <string.h>

#include "scip2.h"
#include "Util.h"


void sendUSBString( unsigned char *data )
{
	for( ; *data != 0; data ++ ){
		sendUSBByte( *data );
	}
}

void sendScipEchoBack( unsigned char *data )
{
	for( ; *data != 0; data ++ ){
		sendUSBByte( *data );
	}
	sendUSBByte( '\n' );
}

void sendScipResponse( unsigned char *data )
{
	unsigned char checksum = 0;
	for( ; *data != 0; data ++ ){
		sendUSBByte( *data );
		checksum += *data;
	}
	sendUSBByte( ( checksum & 0x3F ) + 0x30 );
	sendUSBByte( '\n' );
}

void sendScipResponseF( const char *data )
{
	unsigned char checksum = 0;
	for( ; ; data ++ ){
		unsigned char tmp;
		tmp = pgm_read_byte( data );
		if( tmp == 0 ) break;
		sendUSBByte( tmp );
		checksum += tmp;
	}
	sendUSBByte( ( checksum & 0x3F ) + 0x30 );
	sendUSBByte( '\n' );
}

void sendScipData( unsigned long *data, int len, unsigned char enc )
{
	static const unsigned long enc_mask[4] = { 0x3F, 0xFFF, 0x3FFFF, 0xFFFFFF };
	unsigned char buf[68];
	unsigned char *pbuf;
	
	pbuf = buf;
	for( ; len > 0; len -- ){
		unsigned long tmp;
		unsigned char i;
		unsigned char encorded[4];
		
		tmp = *data;
		data ++;
		if( tmp > enc_mask[enc-1] ){
			// オーバーフロー
			tmp = enc_mask[enc-1];
		}
		for( i = 0; i < enc; i++ ){
			encorded[enc-1-i] = ( tmp & 0x3F ) + 0x30;
			tmp = tmp >> 6;
		}
		for( i = 0; i < enc; i++ ){
			*pbuf = encorded[i];
			pbuf ++;
			if( pbuf - buf >= 64 ){
				*pbuf = 0;
				sendScipResponse( buf );
				pbuf = buf;
			}
		}
	}
	if( pbuf != buf ){
		*pbuf = 0;
		sendScipResponse( buf );
	}
}

int scip2CommandAnalysis( const Scip2CommandType *scip2cmd, unsigned char *command )
{
	unsigned char i, j;
	Scip2CommandType *cmd;
	unsigned char *pcmd;
	unsigned long arg[8];
	
	if( *command == 0 ){
		return 1;
	}
	
	for( cmd = scip2cmd; cmd->Name[0] != 0; cmd++ ){
		if( strncmp( (char*)command, (char*)cmd->Name, strlen( (char*)cmd->Name ) ) == 0 ){
			break;
		}
	}
	if( cmd->Name[0] == 0 ){
		sendScipEchoBack( command );
		sendScipResponse( (unsigned char*)"0E" );
		sendUSBByte( '\n' );
		flushSendStream();
		
		return 1;
	}
	arg[0] = 0;
	j = 0;
	for( i = 0, pcmd = command + strlen( (char*)cmd->Name ); *pcmd != 0; pcmd++ ){
		if( cmd->lArg[i] == 0 ) break;
		arg[i] *= 10;
		arg[i] += *pcmd - '0';
		j ++;
		if( j >= cmd->lArg[i] ){
			i ++;
			arg[i] = 0;
			j = 0;
		}
	}
	return cmd->Func( command, arg );
}