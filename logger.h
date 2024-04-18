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
  class Logger {
    //
    protected :
      //
      static Logger * instances [] ;
      loggerCBs_t & callbacks ;
      //
      Logger (loggerCBs_t & c) : callbacks(c) {}
      //
    public:
      //
      virtual bool loop (unsigned long int) = 0 ;
      virtual void writeHead (void) = 0 ;
      virtual bool writeLine (unsigned long int) = 0 ;
      //
    public :
      //
      static bool loop () ;
      //
  } ;

  template <class A = HardwareSerial> class SerialLogger : public Logger {
    //
    protected :
      //
      A & log ;
      unsigned long flushTime {500} ;
      uint8_t mode {5u} ;
      //
    private : // non-static
      //
      unsigned int loggerSampleInterval {100}, loggerSampleAdjust {3} ;
      unsigned long int loggerNextSampleTime {0} ;
      //
      static bool initialized ;
      //
    protected :
      //
      bool loop (unsigned long int) ;
      void writeHead (void) ;
      bool writeLine (unsigned long int) ;
      void shutdown (void) ;
      //
    public :
      //
      typedef A serial_t ;
      //
      static bool loop () { return Logger::loop () ; }
      static void setup (loggerCBs_t & c, serial_t & s, unsigned int a = 100, unsigned int b = 4) {
        //
        if (!initialized) {
          //
          if (new SerialLogger (c, s, a, b)) {
            //
            initialized = true ;
            //
          }
          //
        }
        //
      }
      //
    protected :
      //
      SerialLogger (loggerCBs_t &, serial_t &, unsigned int = 100, unsigned int = 4) ;
      //
  } ;
  //
  class OpenLogSerialLogger : public SerialLogger <HardwareSerial> {
    //
    static const unsigned long int __retry {3000} ;
    //
    uint8_t pin {static_cast <uint8_t> (rpac::Pin::none)} ;
    //
    public :
      //
      typedef typename SerialLogger::serial_t serial_t ;
      //
      static bool loop () { return Logger::loop () ; }
      static void setup (loggerCBs_t &, serial_t &, rpacPin_t = rpac::Pin::logger, unsigned int a = 100, unsigned int b = 4) ;
      //
    public :
      //
      OpenLogSerialLogger (loggerCBs_t &, serial_t &, rpacPin_t = rpac::Pin::logger, unsigned int a = 100, unsigned int b = 4) ;
      //
  } ;
  //
} ;
//
#endif