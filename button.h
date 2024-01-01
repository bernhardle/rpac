#ifndef __button_h_included__
#define __button_h_included__
//
#include "global.h"
//
const int buttonPin = 16 ;
//
extern bool buttonPressed ;
//
extern void buttonSetup (void) ;
extern bool buttonLoop (void) ;
//
#endif