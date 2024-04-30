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
    const char * data (void) ;
    const char * head (void) ;
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
      static Logger * instances [] ;
      //
      static void stop () {}
      //
    public :
      //
      static bool loop () ;
      //
    private : // non-static
      //
      loggerCBs_t & callbacks ;
      //
      virtual operator bool () const = 0 ;
      //
    protected : // non-static
      //
      Logger (loggerCBs_t &) ;
      virtual bool loop (unsigned long int) = 0 ;
      inline const char * headLine () { return callbacks.head () ; }
      inline const char * dataLine () { return callbacks.data () ; }
      virtual void shutdown () = 0 ;
      //
  } ;
  //
  template <class A = HardwareSerial> class SerialLogger : public Logger {
      //
    private :
      //
      static SerialLogger <A> * instance ;
      static bool initialized ;
      //
    public :
      //
      typedef A serial_t ;
      //
      static void setup (loggerCBs_t &, serial_t & = Serial, unsigned int = 100, unsigned int = 4) ;
      static void stop (void) ;
      //
    private : // non-static
      //
      unsigned int loggerSampleInterval {100}, loggerSampleAdjust {3} ;
      unsigned long int loggerNextSampleTime {0} ;
      uint8_t mode {5u} ;
      //
    protected : // non-static
      //
      A & log ;
      unsigned long flushTime {500} ;
      //
      SerialLogger (loggerCBs_t &, serial_t &, unsigned int = 100, unsigned int = 4) ;
      bool loop (unsigned long int) ;
      void shutdown (void) ;
      bool terminated (void) const { return mode == 5u ; }
      operator bool () const { return mode == 0u ; }
      //
      friend class OpenLogSerialLogger ;
  } ;
  //
  class OpenLogSerialLogger : public Logger {
      //
    private :
      //
      static const unsigned long int __retry {3000} ;
      static OpenLogSerialLogger * instance ;
      static bool initialized ;
      //
    public :
      //
      typedef typename SerialLogger <HardwareSerial>::serial_t serial_t ;
      //
      static void setup (loggerCBs_t &, serial_t & = Serial1, rpacPin_t = rpac::Pin::logger, unsigned int = 100, unsigned int = 4) ;
      static void stop (void) ;
      //
    private : // non-static
      //
      uint8_t pin {static_cast <uint8_t> (rpac::Pin::none)} ;
      SerialLogger <HardwareSerial> * serial {nullptr} ;
      //
      operator bool () const { return serial == nullptr ? false : serial->operator bool() ; } 
      //
    protected : // non-static
      //
      OpenLogSerialLogger (loggerCBs_t &, serial_t &, rpacPin_t, unsigned int, unsigned int) ;
      bool loop (unsigned long int t) ;
      void shutdown (void) ;
      //
  } ;
  //
} ;
//
#endif