//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "relais.h"
#include "logger.h"
//
#if defined(ARDUINO_UBLOX_NINA_W10) || defined(ARDUINO_SEEED_XIAO_RP2040)
inline void __digitalWrite (uint8_t p, int r) { digitalWrite (p, r == HIGH ? LOW : HIGH) ; }
#else
inline void __digitalWrite (uint8_t p, int r) { digitalWrite (p, r) ; }
#endif
//
template <rpacPin_t p> unsigned long rpac::Relais <p>::lastTime {0} ;
template <rpacPin_t p> bool rpac::Relais <p>::lastTrigger {false} ;
template <rpacPin_t p> typename rpac::Relais <p>::state_t rpac::Relais <p>::relais {LOW} ;
//
template <rpacPin_t p> void rpac::Relais <p>::setup (loggerCBs_t & lcbs) {
  //
  pinMode (static_cast <uint8_t> (p), OUTPUT) ;
  //
  __digitalWrite (static_cast <uint8_t> (p), (relais = LOW)) ;
  //
  lcbs.add ([]() -> unsigned long { return static_cast <unsigned long> (relais) ; }, "Relais PIN" + String (static_cast <int> (p), DEC)) ;
  //
}
//
template <rpacPin_t p> bool rpac::Relais <p>::loop (bool trigger) {
  //
  if (relais == HIGH) {
    //
    if (millis () > lastTime + __holdDura) {
      //
      __digitalWrite (static_cast <uint8_t> (p), (relais = LOW)) ;
      //
#ifdef __DEBUG__RELAIS__
      Serial.print ("[Info] Relais state is OFF @ ") ;
      Serial.println (millis (), DEC) ;
      if (trigger) Serial.println ("[WARNING] Trigger still/again set at end of relais hold duration.") ;
#endif
      //
    }
    //
  } else {
    //
    if (trigger && ! lastTrigger) {
      //
      __digitalWrite (static_cast <uint8_t> (p), (relais = HIGH)) ;
      //
#ifdef __DEBUG__RELAIS__
      Serial.print ("[Info] Relais state is ON @ ") ;
      Serial.println (millis (), DEC) ;
#endif
      //
      lastTime = millis () ;
      //
    }
    //
  }
  //
  lastTrigger = trigger ;
  //
  return static_cast <bool> (relais) ;
  //
}
