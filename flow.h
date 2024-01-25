#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
//
const int flowCountsPerUnit = 12 ;
#ifdef __DEBUG__FLOW__
const int flowMirrorPin = 3, flowGroundPin = 4 ;
#endif
//
extern void flowSetup (pin_size_t, loggerCBs_t &) ;
extern bool flowLoop (void) ;
extern unsigned long int flowLastActiveTime (void) ;
//
#endif
