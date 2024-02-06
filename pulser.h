//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#include "global.h"
#include "control.h"
//
extern void pulserSetup (rpacPin_t, controlCBs_t &, loggerCBs_t &) ;
extern bool pulserLoop (bool) ;
//
#endif
