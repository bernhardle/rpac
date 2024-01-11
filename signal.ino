#include "signal.h"
//
static bool signalLastState = false ;
//
void signalSetup (void) {
  //
  pinMode (signalPin, OUTPUT) ;
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