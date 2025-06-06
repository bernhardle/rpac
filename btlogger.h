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
        enum struct Mode {
          ADV_LOG   = 01u,
          LOG       = 02u,
          ADV_ONLY  = 03u,
          SHUTDOWN  = 04u,
          INVENTORY = 05u,
          IDLE = 06u
        } ;
        //
        public :
          //
          using mode_t = Mode ;
          //
        private : // static
          //
          static constexpr uint8_t maxConnectionsBLE {2u} ;
          static constexpr uint8_t maxLoggerLineLength {64u} ;
          //
          static uint16_t connections ;
          static BTLogger * instance ;
          static bool initialized ;
          //
          static void callback (uint16_t, BLECharacteristic *, uint16_t) ;
          //
        private : // non-static
          //
          uint16_t sampleInterval {500u} ;
          uint16_t sampleAdjust {8u} ;
          uint32_t nextSampleTime {0} ;   //
          uint8_t underSample {1u} ;      // divider: logger period = sampleInterval * underSample
          mode_t mode {mode_t::IDLE} ;
          //
          void wrmode (mode_t) ;
          void divide (uint8_t) ;
          operator bool () const { return true ; }
          //
        protected:
          //
          BTLogger (loggerCBs_t &, uint16_t, uint16_t) ;
          bool loop (uint32_t) ;
          bool advertise (void) { mode = mode == mode_t::LOG ? mode_t::ADV_LOG : mode_t::ADV_ONLY ; return true ; }
          void shutdown (void) { mode = mode_t::SHUTDOWN ; }
          //
        public:
          //
          static void setup (loggerCBs_t &, uint16_t = 250u, uint16_t = 4u) ;
          static bool loop (void) { return Logger::loop () ; }
          static void stop (void) { if (instance != nullptr) instance->shutdown () ; }
          static bool start (void) { return instance != nullptr ? instance->advertise () : false ; }
          //
    } ;
    //
} ;
//
inline void rpac::BTLogger::wrmode (mode_t m) {
  //
  mode = m ;
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
  Serial.print ("BTLogger::wrmode () Mode = ") ;
  Serial.print (static_cast <uint8_t> (mode)) ;
  Serial.println (".") ;
#endif
}
//
inline void rpac::BTLogger::divide (uint8_t d) {
  //
  underSample = d < 1 ? 1u : d ;
#if defined(__INFO__LOGGER__) || defined(__DEBUG__LOGGER__)
  Serial.print ("BTLogger::divide () Undersample = ") ;
  Serial.print (underSample) ;
  Serial.println (".") ;
#endif
}
//
#endif
