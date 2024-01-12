#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "pressure.h"
//
class DateTime ;
//
const int loggerPin = 15 ;
const bool loggerFlag = true ;
const unsigned long logRes = 4 ;
//
// extern volatile bool loggerEnabled ;
//
extern void loggerSetup (String &&) ;
extern void loggerLoop (const pressureData_t &, bool, bool, String &&) ;
//
#endif