#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#include "global.h"
//
// extern unsigned int pulserProgressCount ;
//
extern void pulserSetup (pin_size_t, loggerCBs_t &) ;
extern bool pulserLoop (bool) ;
//
#endif
