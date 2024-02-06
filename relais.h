//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __relais_h_included__
#define __relais_h_included__
//
#include "global.h"
//
extern void relaisSetup (rpacPin_t, loggerCBs_t &) ;
extern bool relaisLoop (bool) ;
//
#endif
