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
  pressureSetup (A0, loggerCallBacks) ;       // NanoEvery: A0 || u-blow Nina W-101: A0
  //
  buttonSetup (16, loggerCallBacks) ;         // NanoEvery: 16 || u-blow Nina W-101: 27
  //
  pulserSetup (10, loggerCallBacks) ;         // NanoEvery: 10 || u-blow Nina W-101: 10
  //
  relaisSetup (2, loggerCallBacks) ;          // NanoEvery: 2 || u-blow Nina W-101: 18
  //
  flowSetup (7, loggerCallBacks) ;            // NanoEvery: 7 || u-blow Nina W-101: 19
  //
  loggerSetup (15, loggerCallBacks, start) ;  // NanoEvery: 15 || u-blow Nina W-101: 15
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