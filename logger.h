//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
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
    static const int mcbs = 8, mhdl = 32, rowl = 12 * mcbs ;
    char row [rowl] = "" ;
    uint8_t num = 0 ;
    unsigned long (*cb[mcbs])(void) ;
    char hd [mcbs][mhdl + 1] ;
  public :
    loggerCBs () : num (0) {}
    bool add (unsigned long (*)(void), const String &) ;
    const char * logRow (const String &) ;
    const char * headRow (const String &) ;
    inline unsigned long (*operator [](int i) const)(void) { return cb [i] ; }
  //
} ;
//
const bool loggerFlag = true ;
const unsigned long logRes = 4, loggerRetryDura = 3000 ;
//
extern void loggerSetup (rpac::SimpleButton &, rpacPin_t, controlCBs_t &, loggerCBs_t &, const String & = "No start time argument.") ;
extern bool loggerLoop (loggerCBs_t &) ;
//
#endif