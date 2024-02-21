//
//  (c) Bernhard Schupp, Frankfurt (2024)
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
using Flow = rpac::Flow <rpacPin_t::flow> ;
using Button = rpac::Button <rpacPin_t::button> ;
using Logger = rpac::Logger <10,10> ;
using Pulser = rpac::Pulser <rpacPin_t::pulser> ;
using Signal = rpac::Signal <rpacPin_t::signal> ;
using Relais = rpac::Relais <rpacPin_t::relais> ;
using Control = rpac::Control <rpacPin_t::button> ;
using Pressure = rpac::Pressure <rpacPin_t::pressure> ;
//
loggerCBs_t loggerCallBacks ;
Logger myLog (loggerCallBacks, 100, 8) ;
#ifdef __DEBUG__RPAC__
using Debug = rpac::Logger <11,11> ;
Debug yourLog (loggerCallBacks, 1000, 8) ;
#endif
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
    Signal::Hook hook (Signal::scheme::blinkslow) ;
    const unsigned int waitForCmd{3000} ;
    bool enable{true} ;
    //
#ifdef __DEBUG__RPAC__
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
#ifdef __DEBUG__RPAC__
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
    if (enable) Logger::setup (rpacPin_t::logger, Serial1) ;
    //
  }
  //
#ifdef __DEBUG__RPAC__
  //
  Debug::setup (Serial) ;
  //
  Serial.println ("[INFO] Setup completed.") ;
  //
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
  Control::ctrl_t control = Control::loop (Button::loop(), 4U) ;
  //
  switch (Control::command (control)) {
    //
    case 1 :
      //
      Pulser::autox () ;
      //
      Signal::async (Signal::scheme::blinkfast, 100) ;
      //
      break ;
      //
    case 2 :
      //
      Signal::async (Signal::scheme::blinkfast, 200) ;
      //
      break ;
      //
    case 3 :
      //
      Flow::resox () ;
      //
      Signal::async (Signal::scheme::blinkfast, 300) ;
      //
      break ;
      //
    case 4 :
      //
      Logger::shutdown () ;
      //
      Signal::async (Signal::scheme::blinkfast, 400) ;
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
  //
  Debug::loop (loggerCallBacks) ;
  //
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Loop exceeded " + String (loopMaxDura) + " ms.") ;
  //
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