#include "signal.h"
//
static pin_size_t signalPin ;
static bool signalLastState = false ;
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
  signalLastState = false ;
}
//
void signalLoop (bool state, const signalSequence_t & prio) {
  //
  if (state != signalLastState) digitalWrite (signalPin, (signalLastState = state) ? HIGH : LOW) ;
  //
}