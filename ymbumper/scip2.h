#ifndef __SCIP2_H__
#define __SCIP2_H__

#include <avr/eeprom.h>

typedef struct
{
	unsigned char Name[5];
	unsigned char lArg[8];
	int (*Func)( unsigned char *, unsigned long * );
} Scip2CommandType;

void sendUSBString( unsigned char *data );
void sendScipEchoBack( unsigned char *data );
void sendScipResponse( unsigned char *data );
void sendScipResponseF( const char *data );
void sendScipData( uint32_t *data, int len, unsigned char enc );
void sendScipDataInt( uint32_t *data, uint16_t *data_int,
		int len, unsigned char enc, unsigned char enc_int );

int scip2CommandAnalysis( const Scip2CommandType *scip2cmd, unsigned char *command );

#endif
