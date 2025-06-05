//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#ifndef __btlogger_h_included__
#define __btlogger_h_included__
//
#include "logger.h"
//
class BLECharacteristic ;
//
namespace rpac {
    //
    class BTLogger : public Logger {
        //
        private : // static
          //
          static constexpr uint8_t maxConnectionsBLE {2u} ;
          static constexpr uint8_t maxLoggerLineLength {64u} ;
          //
          static struct hx { bool valid ; uint16_t value; } handles [maxConnectionsBLE] ;
          //
          static BTLogger * instance ;
          static bool initialized ;
          //
          static void callback (uint16_t, BLECharacteristic *, uint16_t) ;
          //
        private : // non-static
          //
          uint32_t sampleInterval {500u}, sampleAdjust {8u} ;
          uint32_t nextSampleTime {0} ;
          uint8_t mode {5u} ;
          //
          void wrmode (uint8_t m) { mode = m ; }
          operator bool () const { return true ; }
          //
        protected:
          //
          BTLogger (loggerCBs_t &, unsigned int, unsigned int) ;
          bool loop (unsigned long int) ;
          void shutdown () { mode = 3u ; }
          //
        public:
          //
          static void setup (loggerCBs_t &, unsigned int = 250, unsigned int = 4) ;
          static bool loop () { return Logger::loop () ; }
          static void stop () { if (instance != nullptr) instance->shutdown () ; }
          static void advertise () { if (instance != nullptr) instance->wrmode (1u) ; }
          //
    } ;
    //
} ;
//
#endif
