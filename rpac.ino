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
#endif
//
using Time = rpac::Time <RTC_PCF8523> ;
using Flow = rpac::Flow <rpac::Pin::flow> ;
using Button = rpac::Button <rpac::Pin::button> ;
using Logger = rpac::Logger <rpac::Pin::logger> ;
using Pulser = rpac::Pulser <rpac::Pin::pulser> ;
using Signal = rpac::Signal <rpac::Pin::signal> ;
using Relais = rpac::Relais <rpac::Pin::relais> ;
using Control = rpac::Control <rpac::Pin::button> ;
using Pressure = rpac::Pressure <rpac::Pin::pressure> ;
//
loggerCBs_t loggerCallBacks ;
Logger myLog (loggerCallBacks, 100, 3) ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  Signal::setup () ;
  //
  Time::setup (loggerCallBacks) ;
  //
  Pressure::setup (loggerCallBacks) ;
  //
  Button::setup (loggerCallBacks) ;
  //
  Control::setup (loggerCallBacks) ;
  //
  Pulser::setup (loggerCallBacks) ;
  //
  Relais::setup (loggerCallBacks) ;
  //
  Flow::setup (loggerCallBacks) ;
  //
  {
    typename Signal::Hook hook (Signal::scheme::blinkslow) ;
    const unsigned int waitForCmd{3000} ;
    bool enable{true} ;
    //
#ifdef __DEBUG__LOGGER__
    Serial.print ("[INTERACTIVE] To disable logging button within ") ;
    Serial.print (String (waitForCmd / 1000, DEC)) ;
    Serial.println (" sec.") ;
#endif
    //
    for (unsigned long mytime = millis () ; mytime + waitForCmd > millis () ; delay (5)) {
      //
      Signal::loop (false) ;
      //
      if (Control::command (Control::loop (Button::loop (), 1)) == 1) {
        //
        enable = false ;
        //
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INTERACTIVE] Button has been pressed ...") ;
#endif
        //
        Signal::blocking (Signal::scheme::blinkfast, 1000) ;
        //
        break ;
        //
      }
      //
    }
    //
    Logger::setup (enable) ;
    //
  }
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
  typename Control::ctrl_t control ;
  //
#ifdef __DEBUG__RPAC__
  unsigned long loopBegin = millis () ;
#endif
  //
  control = Control::loop (Button::loop(), 4U) ;
  //
  switch (Control::command (control)) {
    //
    case 1 :
      //
      Signal::async (Signal::scheme::blinkfast, 40) ;
      //
      break ;
      //
    case 2 :
      //
      Pulser::autox () ;
      //
      break ;
      //
    case 3 :
      //
      Flow::resox () ;
      //
      Signal::async (Signal::scheme::blinkfast, 120) ;
      //
      break ;
      //
    case 4 :
      //
      Logger::shutdown () ;
      //
      break ;
      //
  }
  //
  Pressure::loop () ;
  //
  Relais::loop (Flow::loop ()) ;
  //
  Logger::loop (loggerCallBacks) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
  Signal::loop (Pulser::loop (Control::trigger(control))) ;
  //
#ifdef ARDUINO_UBLOX_NINA_W10
  //  Prevent on RTOS watchdog from firing ...
  delay (1) ;
  //
#endif
}
//