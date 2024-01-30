#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
#include "logger.h"
#include "control.h"
//
extern void flowSetup (pin_size_t, controlCBs_t &, loggerCBs_t &) ;
extern bool flowLoop (void) ;
extern unsigned long int flowLastActiveTime (void) ;
//
#endif
