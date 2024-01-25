#ifndef __pressure_h_included__
#define __pressure_h_included__
//
#include <String.h>
//
#include "global.h"
//
extern void pressureSetup (pin_size_t, loggerCBs_t &) ;
extern void pressureLoop (void) ;
//
#endif