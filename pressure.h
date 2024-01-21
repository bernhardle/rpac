#ifndef __pressure_h_included__
#define __pressure_h_included__
//
#include <String.h>
//
#include "global.h"
//
const int pressurePin = A0 ;
//
extern void pressureSetup (loggerCBs_t &) ;
extern void pressureLoop (void) ;
//
#endif