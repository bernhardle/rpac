//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "button.h"
#include "control.h"
//
class loggerCBs {
    //  
    static const int mcbs = 8 ;             //  mcbs: max. number of callback functions
    static const int mhdl = 64 ;            //  mhdl: max. length of header string
    static const int rowl = mhdl * mcbs ;   //  rowl: lenght of buffer used by sprintft
    //
    char row [rowl]{0} ;
    uint8_t num = 0 ;
    unsigned long (*cb[mcbs])(void) ;
    char hd [mcbs][mhdl + 1]{0} ;
    //
  public :
    loggerCBs () : num (0) {}
    bool add (unsigned long (*)(void), const String &) ;
    const char * logRow (const String &) ;
    const char * headRow (const String &) ;
    inline unsigned long (*operator [](int i) const)(void) { return cb [i] ; }
  //
} ;
//
namespace rpac {
  //
  class Logger {
    //
    protected :
      //
      const static unsigned short int loggerRetryDura{3000}, waitForCmd{4000}, loggerSampleInterval{100} ;
      const static uint8_t  loggerSampleAdjust{4} ;
#ifdef __DEBUG__LOGGER__
      const static unsigned short int loggerSampleOutput{10 * loggerSampleInterval} ;
#endif
      static unsigned long loggerNextSampleTime, loggerShutdownFlushTime ;
      static uint8_t mode ;
      static uint8_t pin ;
      static bool enable ;
      //
      static uint8_t disable (uint8_t) ;
      static uint8_t shutdown (uint8_t) ;
      //
    public :
      //
      template <rpac::rpacPin_t p, rpac::rpacPin_t s> static void setup (rpacPin_t, rpac::controlCBs_t & a, loggerCBs_t & b, const String & = "No start time argument.") ;
      static bool loop (loggerCBs_t &) ;
      //
  } ;
  //
} ;
#endif