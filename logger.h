#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "pressure.h"
//
class loggerCBs {
  //
  public :
    static const int mcbs = 8, mhdl = 32, rowl = 256 ;
    char row [rowl] = "" ;
    int num = 0 ;
    unsigned long (*cb[mcbs])(void) ;
    char hd [mcbs][mhdl + 1] ;
  public :
    loggerCBs () : num (0) {}
    bool add (unsigned long (*)(void), const String &) ;
    const char * logRow (const String &) ;
    const char * headRow (const String &) ;
    inline unsigned long (*operator [](int i))(void) const { return cb [i] ; }
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
extern void loggerSetup (const loggerCBs_t &, const String & = "No start time argument.") ;
extern bool loggerLoop (const String &, const loggerCBs_t &, bool = false) ;
//
#endif