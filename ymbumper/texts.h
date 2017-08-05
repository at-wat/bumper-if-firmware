#include <avr/eeprom.h>

#ifndef FIRM_VER
#define FIRM_VER "DEBUG_VER"
#endif

const char VV_VEND[] PROGMEM = "VEND:Atsushi Watanabe;";
const char VV_PROD[] PROGMEM = "PROD:Bumper Interface;";
const char VV_FIRM[] PROGMEM = "FIRM:" FIRM_VER ";";
const char VV_PROT[] PROGMEM = "PROT:SCIP 2.1;";
const char VV_SERI_FMT[] = "SERI:%s;";

const char PP_MODL[] PROGMEM = "MODL:BumperIF-2017;";
const char PP_DMIN[] PROGMEM = "DMIN:1;";
const char PP_DMAX_FMT[] = "DMAX:%d;";
const char PP_AMIN[] PROGMEM = "AMIN:0;";
const char PP_AMAX_FMT[] = "AMAX:%d;";
const char PP_AFRT_FMT[] = "AFRT:%d;";
const char PP_ARES_FMT[] = "ARES:%d;";
const char PP_SCAN[] PROGMEM = "SCAN:6000;";

const char TIME_FMT[] =  "TIME:%06X;";
const char MD_FMT[] = "M%c%04d%04d%02d%01d%02d";
const char RW_FMT[] = "%04d(%04d) %04d(%04d) %04d(%04d) %04d(%04d) %04d(%04d) %04d(%04d) \n";
