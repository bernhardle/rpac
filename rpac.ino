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
unsigned loopCount = 0 ;
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
  signalSetup (rpacPin::signal) ;                                     //  NanoEvery: LED_BUILTIN || u-blow Nina W-101: LED_BLUE
  //
  String start = timeSetup (loggerCallBacks) ;                        //
  //
  pressureSetup (rpacPin::pressure, loggerCallBacks) ;                // NanoEvery: A0 || u-blow Nina W-101: A0
  //
  buttonSetup (rpacPin::button, loggerCallBacks) ;                    // NanoEvery: 16 || u-blow Nina W-101: 27 = SW01
  //
  controlSetup (controlCallBacks, loggerCallBacks) ;                  //
  //
  pulserSetup (rpacPin::pulser, controlCallBacks, loggerCallBacks) ;  // NanoEvery: 10 || u-blow Nina W-101: 10
  //
  relaisSetup (rpacPin::relais, loggerCallBacks) ;                    // NanoEvery: 2 || u-blow Nina W-101: 18
  //
  flowSetup (rpacPin::flow, controlCallBacks, loggerCallBacks) ;      // NanoEvery: 7 || u-blow Nina W-101: 33 = SW02
  //
  loggerSetup (rpacPin::logger, controlCallBacks, loggerCallBacks, start) ;  // NanoEvery: 15 || u-blow Nina W-101: 15
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  bool button = false, flow = false, pulse = false, relais = false ;
  //
  button = controlLoop (buttonLoop (), controlCallBacks) ;
  //
  pulse = pulserLoop (button) ;
  //
  pressureLoop () ;
  //
  flow = flowLoop () ;
  //
  relais = relaisLoop (flow) ;
  //
  loggerLoop (loggerCallBacks) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  signalLoop ((relais && ! pulse) || (pulse && ! relais)) ;
  //
#ifdef ARDUINO_UBLOX_NINA_W10
  //  Prevent watchdog from firing ...
  delay (1) ;
#endif
}
//