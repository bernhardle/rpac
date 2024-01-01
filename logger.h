#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "sensor.h"
//
const int loggerPin = 15 ;
const bool loggerFlag = true ;
const unsigned long logRes = 4 ;
//
// extern volatile bool loggerEnabled ;
//
extern void loggerSetup (void) ;
extern void loggerLoop (const sensorData_t &, String &&) ;
//
#endif