//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef ARDUINO_UBLOX_NINA_W10
#include <EEPROM.h>
#endif
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
rpac::Logger <rpac::Pin::logger> myLog (loggerCallBacks, 100, 3) ;
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
  rpac::Logger <rpac::Pin::logger>::setup <rpac::Pin::button, rpac::Pin::signal> (controlCallBacks) ;
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
  //
  rpac::Pressure <rpac::Pin::pressure>::loop () ;
  //
  rpac::Relais <rpac::Pin::relais>::loop (rpac::Flow <rpac::Pin::flow>::loop ()) ;
  //
  rpac::Logger <rpac::Pin::logger>::loop (loggerCallBacks) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  rpac::Signal <rpac::Pin::signal>::loop (rpac::Pulser<rpac::Pin::pulser>::loop (rpac::Control <rpac::Pin::button, rpac::Pin::signal>::loop (rpac::Button <rpac::Pin::button>::loop(), controlCallBacks))) ;
  //
#ifdef ARDUINO_UBLOX_NINA_W10
  //  Prevent watchdog from firing ...
  delay (1) ;
  //
#endif
}
//