//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __pressure_h_included__
#define __pressure_h_included__
//
#include <String.h>
//
#include "global.h"
//
extern void pressureSetup (rpacPin_t, loggerCBs_t &) ;
extern void pressureLoop (void) ;
//
#endif