#ifndef __signal_h_included__
#define __signal_h_included__
//
const int signalPin = LED_BUILTIN ;
//
extern void signalSetup (void) ;
extern void signalLoop (bool) ;
//
#endif