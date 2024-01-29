#include <EEPROM.h>
//
#include "global.h"
#include "button.h"
#include "control.h"
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
controlCBs_t controlCallBacks ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  signalSetup (LED_BUILTIN) ;                         //  NanoEvery: LED_BUILTIN || u-blow Nina W-101: LED_BLUE
  //
  String start = timeSetup (loggerCallBacks) ;
  //
  pressureSetup (A0, loggerCallBacks) ;               // NanoEvery: A0 || u-blow Nina W-101: A0
  //
  buttonSetup (16, loggerCallBacks) ;                 // NanoEvery: 16 || u-blow Nina W-101: 27
  //
  controlSetup (controlCallBacks, loggerCallBacks) ;  //
  //
  pulserSetup (10, controlCallBacks, loggerCallBacks) ; // NanoEvery: 10 || u-blow Nina W-101: 10
  //
  relaisSetup (2, loggerCallBacks) ;                  // NanoEvery: 2 || u-blow Nina W-101: 18
  //
  flowSetup (7, loggerCallBacks) ;                    // NanoEvery: 7 || u-blow Nina W-101: 19
  //
  loggerSetup (15, controlCallBacks, loggerCallBacks, start) ;  // NanoEvery: 15 || u-blow Nina W-101: 15
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  bool buttonState = false, flowState = false, pulseState = false, relaisState = false, shutdown = false ;
  //
  // shutdown = loopBegin > flowLastActiveTime() + 60000 ? true : false ;
  //
  buttonState = controlLoop (buttonLoop (), controlCallBacks) ;
  //
  pulseState = pulserLoop (buttonState) ;
  //
  pressureLoop () ;
  //
  flowState = flowLoop () ;
  //
  relaisState = relaisLoop (flowState) ;
  //
  loggerLoop ("", loggerCallBacks) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  signalLoop ((relaisState && ! pulseState) || (pulseState && ! relaisState)) ;
  //
}
//