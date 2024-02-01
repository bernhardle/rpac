#include "relais.h"
#include "logger.h"
//
typedef decltype (HIGH) relaisState_t ;
//
#ifdef ARDUINO_UBLOX_NINA_W10
inline void __digitalWrite (uint8_t p, int r) { digitalWrite (p, r == HIGH ? LOW : HIGH) ; }
#else
inline void __digitalWrite (uint8_t p, int r) { digitalWrite (p, r) ; }
#endif
//
const unsigned int relaisHoldDura = 100 ;
//
static unsigned long int relaisLastTime ;
static bool relaisOn = false, relaisLastTrigger = false ;
static relaisState_t relaisState = LOW ;
static uint8_t relaisPin ;
//
static unsigned long int relaisDataCB (void) {
  //
  return relaisState ;
  //
}
//
void relaisSetup (rpacPin_t pin, loggerCBs_t & callbacks) {
  //
  pinMode ((relaisPin = static_cast <uint8_t> (pin)), OUTPUT) ;
  __digitalWrite (relaisPin, (relaisState = LOW)) ;
  //
  callbacks.add (& relaisDataCB, "relais") ;
  //
}
//
bool relaisLoop (bool trigger) {
  //
  if (relaisState == HIGH) {
    //
    if (millis () > relaisLastTime + relaisHoldDura) {
      //
      __digitalWrite (relaisPin, (relaisState = LOW)) ;
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
    if (trigger && ! relaisLastTrigger) {
      //
      __digitalWrite (relaisPin, (relaisState = HIGH)) ;
      //
#ifdef __DEBUG__RELAIS__
      Serial.print ("[Info] Relais state is ON @ ") ;
      Serial.println (millis (), DEC) ;
#endif
      //
      relaisLastTime = millis () ;
      //
    }
    //
  }
  //
  relaisLastTrigger = trigger ;
  //
  return relaisState ;
  //
}
