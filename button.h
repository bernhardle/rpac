#ifndef __button_h_included__
#define __button_h_included__
//
#include "global.h"
//
extern volatile unsigned long buttonPressedTime ;
//
extern void buttonSetup (pin_size_t, loggerCBs_t &) ;
extern bool buttonLoop (void) ;
//
#endif