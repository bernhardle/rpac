#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
//
const int flowPin = 7, flowCountsPerUnit = 12 ;
//
extern void flowSetup (loggerCBs_t &) ;
extern bool flowLoop (void) ;
//
#endif
