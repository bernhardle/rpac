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
    const char * logRow (void) ;
    const char * headRow (void) ;
    inline unsigned long (*operator [](int i) const)(void) { return cb [i] ; }
  //
} ;
//
namespace rpac {
  //
  template <rpac::rpacPin_t p> class Logger {
    //
    const static unsigned short int loggerRetryDura{3000}, waitForCmd{4000} ;
#ifdef __DEBUG__LOGGER__
    // const static unsigned short int loggerSampleOutput{10 * loggerSampleInterval} ;
#endif
    static Logger *instances[10] ;
    static unsigned long loggerShutdownFlushTime ;
    static uint8_t mode ;
    static uint8_t pin ;
    static bool enable ;
    //
    public :
      //
      template <rpac::rpacPin_t b, rpac::rpacPin_t s> static void setup (rpac::controlCBs_t &) ;
      static bool loop (loggerCBs_t &) ;
      //
    private : // non-static
      //
      unsigned int loggerSampleInterval{100}, loggerSampleOutput{1000}, loggerSampleAdjust{4} ;
      unsigned long int loggerNextSampleTime{0} ;
      loggerCBs_t & callbacks ;
      //
    public :
      //
      Logger (loggerCBs_t &, unsigned int a = 100, unsigned int b = 4) ;
      void writeHead (HardwareSerial &) ;
      bool writeLog (unsigned long int) ;
#ifdef __DEBUG__LOGGER__
      void writeDebug (unsigned long int) ;
#endif
      //
  } ;
  //
} ;
#endif