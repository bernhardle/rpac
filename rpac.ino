//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include <EEPROM.h>
//
#include <RTClib.h>
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
rpac::controlCBs_t controlCallBacks ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  rpac::Signal <rpac::Pin::signal>::setup (controlCallBacks) ;
  //
  rpac::Time <RTC_PCF8523>::setup (loggerCallBacks) ;
  //
  rpac::Pressure <rpac::Pin::pressure>::setup (loggerCallBacks) ;
  //
  rpac::Button <rpac::Pin::button>::setup (loggerCallBacks) ;
  //
  rpac::Control <rpac::Pin::button, rpac::Pin::signal>::setup (loggerCallBacks) ;
  //
  rpac::Pulser<rpac::Pin::pulser>::setup (controlCallBacks, loggerCallBacks) ;
  //
  rpac::Relais <rpac::Pin::relais>::setup (loggerCallBacks) ;
  //
  rpac::Flow <rpac::Pin::flow>::setup (controlCallBacks, loggerCallBacks) ;
  //
  rpac::Logger::setup <rpac::Pin::button, rpac::Pin::signal> (rpac::Pin::logger, controlCallBacks, loggerCallBacks, "tbd") ;  // NanoEvery: 15 || u-blow Nina W-101: 15
  //
#ifdef __DEBUG__RPAC__
  Serial.println ("[INFO] Setup completed.") ;
#endif
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
#ifdef __DEBUG__RPAC__
  unsigned long loopBegin = millis () ;
#endif
  bool button = false, pulse = false ;
  //
  button = rpac::Control <rpac::Pin::button, rpac::Pin::signal>::loop (controlCallBacks) ;
  //
  pulse = rpac::Pulser<rpac::Pin::pulser>::loop (button) ;
  //
  rpac::Pressure <rpac::Pin::pressure>::loop () ;
  //
  rpac::Relais <rpac::Pin::relais>::loop (rpac::Flow <rpac::Pin::flow>::loop ()) ;
  //
  rpac::Logger::loop (loggerCallBacks) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  rpac::Signal <rpac::Pin::signal>::loop (pulse) ;
  //
#ifdef ARDUINO_UBLOX_NINA_W10
  //  Prevent watchdog from firing ...
  delay (1) ;
  //
#endif
}
//