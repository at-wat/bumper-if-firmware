#include <avr/eeprom.h>

const char VV_VEND[] PROGMEM = "VEND:Tohoku Univ., Atsushi Watanabe;";
const char VV_PROD[] PROGMEM = "PROD:Bumper Interface;";
const char VV_FIRM[] PROGMEM = "FIRM:2016.08.31;";
const char VV_PROT[] PROGMEM = "PROT:SCIP 2.0;";
const char VV_SERI_FMT[] = "SERI:%s;";

const char PP_MODL[] PROGMEM = "MODL:BumperIF-8;";
const char PP_DMIN[] PROGMEM = "DMIN:1;";
const char PP_DMAX_FMT[] = "DMAX:%d;";
const char PP_AMIN[] PROGMEM = "AMIN:0;";
const char PP_AMAX[] PROGMEM = "AMAX:7;";
const char PP_ARES_FMT[] = "ARES:%d;";
const char PP_AFRT[] PROGMEM = "AFRT:0;";
const char PP_SCAN[] PROGMEM = "SCAN:100;";

const char TIME_FMT[] =  "TIME:%06X;";
const char MD_FMT[] = "M%c%04d%04d%02d%01d%02d";
const char RW_FMT[] = "%04d(%04d) %04d(%04d) %04d(%04d) %04d(%04d) %04d(%04d) %04d(%04d) \n";
