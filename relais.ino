#include "relais.h"
#include "logger.h"
//
typedef decltype (HIGH) relaisState_t ;
//
const unsigned int relaisHoldDura = 100 ;
//
static unsigned long int relaisLastTime ;
static bool relaisOn = false, relaisLastTrigger = false ;
static relaisState_t relaisState = LOW ;
static pin_size_t relaisPin ;
//
static unsigned long int relaisDataCB (void) {
  //
  return relaisState ;
  //
}
//
void relaisSetup (pin_size_t pin, loggerCBs_t & callbacks) {
  //
  pinMode ((relaisPin = pin), OUTPUT) ;
  digitalWrite (relaisPin, (relaisState = LOW)) ;
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
      digitalWrite (relaisPin, (relaisState = LOW)) ;
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
      digitalWrite (relaisPin, (relaisState = HIGH)) ;
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
