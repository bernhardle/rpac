#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
const int pulserPin = 10 ;
//
extern volatile unsigned long pulserChangeTime ;
// extern volatile bool pulserEnabled ;
extern unsigned int pulserMode, pulserProgressCount ;
//
extern void pulserSetup (volatile unsigned long &) ;
extern void pulserLoop (volatile unsigned long &, unsigned int &) ;
//
#endif
