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
    unsigned long (*cb[mcbs])(void){nullptr} ;
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
  template <class A, int n> class SerialLogger {
    //
    typedef A serial_t ;
    //
    const static unsigned short __retry {3000} ;
    //
    // static byte space [sizeof (std::reference_wrapper <A>)] ;
    static std::reference_wrapper <A> * log ;
    static SerialLogger *instances [n] ;
    static unsigned long flushTime ;
    static uint8_t mode ;
    static uint8_t pin ;
    //
    public :
      //
      static bool setup (serial_t &) ;
      static bool setup (rpacPin_t, serial_t &) ;
      static bool loop (loggerCBs_t &) ;
      static void shutdown (void) ;
      //
    private : // non-static
      //
      unsigned int loggerSampleInterval{100}, loggerSampleAdjust{3} ;
      unsigned long int loggerNextSampleTime{0} ;
      loggerCBs_t & callbacks ;
      //
    protected :
      //
      void writeHead (void) ;
      bool writeLine (unsigned long int) ;
      //
    public :
      //
      SerialLogger (loggerCBs_t &, unsigned int a = 100, unsigned int b = 4) ;
      //
  } ;
  //
} ;
//
#endif