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
loggerCBs_t loggerCallBacks ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  signalSetup () ;
  //
  buttonSetup (loggerCallBacks) ;
  //
  loggerSetup (rtcSetup ()) ;
  //
  pressureSetup (loggerCallBacks) ;
  //
  pulserSetup (loggerCallBacks) ;
  //
  relaisSetup (loggerCallBacks) ;
  //
  flowSetup (loggerCallBacks) ;
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  bool pulseState, buttonState, flowState, relaisState ;
  //
  buttonState = buttonLoop () ;
  //
  pulseState = pulserLoop (buttonState) ;
  //
  flowState = flowLoop () ;
  //
  pressureLoop (pressureData) ;
  //
  relaisState = relaisLoop (flowState) ;
  //
  loggerLoop (pressureData, pulseState, flowState, "[]", loggerCallBacks) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  signalLoop ((relaisState && ! pulseState) || (pulseState && ! relaisState)) ;
  //
}
//