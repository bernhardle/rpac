#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
//
const int flowPin = 7, flowCountsPerLitre = 119 ;
//
extern void flowSetup (void) ;
extern bool flowLoop (void) ;
//
#endif
