#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "control.h"
//
class loggerCBs {
  //
  //  mcbs: max. number of callback functions
  //  mhdl: max. length of header string
  //  mfml: max. length of format string
  //  rowl: lenght of buffer used by sprintft
  //  
  public :
    static const int mcbs = 8, mhdl = 32, mfml = 8, rowl = 256 ;
    char row [rowl] = "" ;
    int num = 0 ;
    unsigned long (*cb[mcbs])(void) ;
    char hd [mcbs][mhdl + 1], fm [mcbs][mfml + 1] ;
  public :
    loggerCBs () : num (0) {}
    bool add (unsigned long (*)(void), const String &) ;
    const char * logRow (const String &) ;
    const char * headRow (const String &) ;
    inline unsigned long (*operator [](int i) const)(void) { return cb [i] ; }
  //
} ;
//
class DateTime ;
//
const bool loggerFlag = true ;
const unsigned long logRes = 4, loggerRetryDura = 3000 ;
//
// extern volatile bool loggerEnabled ;
//
extern void loggerSetup (pin_size_t, controlCBs_t &, loggerCBs_t &, const String & = "No start time argument.") ;
extern bool loggerLoop (loggerCBs_t &) ;
//
#endif