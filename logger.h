//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __logger_h_included__
#define __logger_h_included__
//
#include "button.h"
//
class loggerCBs {
    //  
    static const int mcbs{8} ;             //  mcbs: max. number of callback functions
    static const int mhdl{64} ;            //  mhdl: max. length of header string
    static const int rowl{mhdl * mcbs} ;   //  rowl: lenght of buffer used by sprintft
    //
    char row [rowl]{0} ;
    unsigned int num{0} ;
    unsigned long (*cb[mcbs])(void) ;
    char hd [mcbs][mhdl + 1]{0} ;
    //
  public :
    loggerCBs () : num {0} {}
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
    const static unsigned short loggerRetryDura{3000} ;
    //
    static Logger *instances[10] ;
    static unsigned long loggerShutdownFlushTime ;
    static uint8_t mode ;
    static uint8_t pin ;
    static bool enable ;
    //
    public :
      //
      static void setup (bool) ;
      static bool loop (loggerCBs_t &) ;
      static void shutdown (void) ;
      //
    private : // non-static
      //
      unsigned int loggerSampleInterval{100}, loggerSampleOutput{1000}, loggerSampleAdjust{4} ;
      unsigned long int loggerNextSampleTime{0} ;
      loggerCBs_t & callbacks ;
      //
    protected :
      //
      void writeHead (HardwareSerial &) ;
      bool writeLog (unsigned long int) ;
#ifdef __DEBUG__LOGGER__
      void writeDebug (unsigned long int) ;
#endif
      //
    public :
      //
      Logger (loggerCBs_t &, unsigned int a = 100, unsigned int b = 4) ;
      //
  } ;
  //
} ;
#endif