#include "signal.h"
//
static pin_size_t signalPin ;
static bool signalState = false ;
//
void signalSetup (pin_size_t pin) {
  //
  pinMode ((signalPin = pin), OUTPUT) ;
  //
  digitalWrite (signalPin, HIGH) ;
  //
  delay (1500) ;
  //
  digitalWrite (signalPin, LOW) ;
  //
  delay (500) ;
  //
  signalState = false ;
}
//
void signalLoop (bool state, const signalSequence_t & prio) {
  //
  if (state != signalState) {
    //
    signalState = state ;
    //
  #ifdef ARDUINO_UBLOX_NINA_W10
    digitalWrite (signalPin, ! signalState) ;
  #else
    digitalWrite (signalPin, signalState) ;
  #endif
    //
  }
  //
}