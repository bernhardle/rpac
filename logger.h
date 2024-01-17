#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "pressure.h"
//
class loggerCBs {
  //
  public :
    static const int mcbs = 8 ;
    int num ;
    unsigned long (*cb[mcbs])(void) ;
  public :
    loggerCBs () : num (0) {}
    bool add (unsigned long (*)(void)) ;
  //
} ;
//
class DateTime ;
//
const int loggerPin = 15 ;
const bool loggerFlag = true ;
const unsigned long logRes = 4, loggerRetryDura = 3000 ;
//
// extern volatile bool loggerEnabled ;
//
extern void loggerSetup (String &&) ;
extern void loggerLoop (const pressureData_t &, bool, bool, String &&, const loggerCBs_t &) ;
//
#endif