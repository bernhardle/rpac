#ifndef __pressure_h_included__
#define __pressure_h_included__
//
#include <String.h>
//
#include "global.h"
//
const unsigned long sampleInterval = 250 ;
const int pressurePin = A0 ;
//
typedef class pressureData {
  //
  public :
    unsigned long time ;
    unsigned short sample, pressure ;
  public :
    pressureData () : time (0), sample (0), pressure (0) {}
  //
} pressureData_t ;
//
extern void pressureSetup (loggerCBs_t &) ;
extern void pressureLoop (pressureData_t &) ;
//
#endif