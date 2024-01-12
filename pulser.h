#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#include "global.h"
//
const int pulserPin = 10 ;
//
// extern unsigned int pulserProgressCount ;
//
extern void pulserSetup (void) ;
extern bool pulserLoop (bool) ;
//
#endif
