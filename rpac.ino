#include <EEPROM.h>
//
#include "global.h"
#include "button.h"
#include "flow.h"
#include "pulser.h"
#include "pressure.h"
#include "relais.h"
#include "logger.h"
#include "signal.h"
#include "time.h"
//
#ifdef __DEBUG__RPAC__
const unsigned long loopMaxDura = 12 ;
#endif
//
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
  String start = timeSetup (loggerCallBacks) ;
  //
  pressureSetup (A0, loggerCallBacks) ;
  //
  buttonSetup (16, loggerCallBacks) ;
  //
  pulserSetup (10, loggerCallBacks) ;
  //
  relaisSetup (2, loggerCallBacks) ;
  //
  flowSetup (7, loggerCallBacks) ; 
  //
  loggerSetup (15, loggerCallBacks, start) ;
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  bool buttonState, flowState, pulseState, relaisState, shutdown = false ;
  //
  shutdown = loopBegin > flowLastActiveTime() + 30000 ? true : false ;
  //
  buttonState = buttonLoop () ;
  //
  pulseState = pulserLoop (buttonState) ;
  //
  flowState = flowLoop () ;
  //
  pressureLoop () ;
  //
  relaisState = relaisLoop (flowState) ;
  //
  loggerLoop ("", loggerCallBacks, shutdown) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  signalLoop ((relaisState && ! pulseState) || (pulseState && ! relaisState)) ;
  //
}
//