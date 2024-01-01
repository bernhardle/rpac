#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
const int pulserPin = 10 ;
//
extern unsigned long pulserChangeTime ;
extern unsigned int pulserProgressCount ;
//
extern void pulserSetup (void) ;
extern bool pulserLoop (unsigned long &, unsigned int &, bool) ;
//
#endif
