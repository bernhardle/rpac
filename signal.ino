#include "signal.h"
//
static bool lastLEDState = false ;
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
  lastLEDState = false ;
}
//
void signalLoop (bool state) {
  //
  if (state != lastLEDState) digitalWrite (signalPin, (lastLEDState = state) ? HIGH : LOW) ;
  //
}