//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#ifndef __btlogger_h_included__
#define __btlogger_h_included__
//
#include "logger.h"
//
namespace rpac {
    //
    class BTLogger : public Logger {
        //
        private : // static
          //
          static constexpr uint8_t maxConnectionsBLE {2} ;
          //
          static BTLogger * instance ;
          static bool initialized ;
          //
        private : // non-static
          //
          const static uint16_t maxLoggerLineLength {64} ;
          //
          unsigned long int sampleInterval {500}, sampleAdjust {8} ;
          unsigned long int nextSampleTime {0} ;
          uint8_t mode {5u} ;
          //
          void wrmode (uint8_t m) { mode = m ; }
          operator bool () const { return true ; }
          //
        protected:
          //
          BTLogger (loggerCBs_t &, unsigned int, unsigned int) ;
          bool loop (unsigned long int) ;
          void shutdown () { } 
          //
        public:
          //
          static void setup (loggerCBs_t &, unsigned int = 250, unsigned int = 4) ;
          static bool loop () { return Logger::loop () ; }
          static void stop () { if (instance != nullptr) instance->shutdown () ; }
          //
    } ;
    //
} ;
//
#endif
