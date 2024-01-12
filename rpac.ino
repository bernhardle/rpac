#include <EEPROM.h>
//
#include "global.h"
#include "button.h"
#include "pulser.h"
#include "pressure.h"
#include "logger.h"
#include "signal.h"
#include "rtc.h"
//
#ifdef __DEBUG__RPAC__
const unsigned long loopMaxDura = 8 ;
#endif
//
pressureData_t pressureData ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  signalSetup () ;
  //
  buttonSetup () ;
  //
  loggerSetup (setupRTC ()) ;
  //
  pressureSetup (pressureData) ;
  //
  pulserSetup () ;
  //
  relaisSetup () ;
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  bool pulseState, buttonState, flowState ;
  //
  buttonState = buttonLoop () ;
  //
  pulseState = pulserLoop (buttonState) ;
  //
  signalLoop (pulseState) ;
  //
  pressureLoop (pressureData) ;
  //
  flowState = flowLoop () ;
  //
  loggerLoop (pressureData, pulseState, flowState, "[]") ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  relaisLoop (buttonState) ;
  //
}
//