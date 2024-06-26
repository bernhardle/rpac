//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "button.h"
#include "control.h"
#include "flow.h"
#include "pulser.h"
#include "pressure.h"
#include "relais.h"
#include "logger.h"
#include "fslogger.h"
#include "signaling.h"
#include "rtc.h"
//
#ifdef __DEBUG__RPAC__
const unsigned long loopMaxDura {12} ;
#endif
//
using Time = rpac::Time <RTC_PCF8523> ;
using Flow = rpac::Flow <rpacPin_t::flow> ;
//
#if defined(ARDUINO_SEEED_XIAO_RP2040)
using Data = rpac::FlashLogger ;
#elif defined(ARDUINO_AVR_NANO_EVERY) 
using Data = rpac::OpenLogSerialLogger ;
#else
using Data = rpac::SerialLogger <decltype (Serial)> ;
#endif
//
#ifdef __DEBUG__RPAC__
using Debug = rpac::SerialLogger <decltype (Serial)> ;
#endif
using Logger = rpac::Logger ;
using Button = rpac::Button <rpacPin_t::button> ;
using Pulser = rpac::Pulser <rpacPin_t::pulser> ;
using Signal = rpac::Signal <rpacPin_t::signal> ;
using Relais = rpac::Relais <rpacPin_t::relais> ;
using Control = rpac::Control ;
using Pressure = rpac::Pressure <rpacPin_t::pressure> ;
//
static loggerCBs_t callBacks ;
//
// the setup function runs once when you press reset or power the board
//
void setup () {
  //
  Serial.begin (115200) ;
  //
#if defined(__DEBUG__RPAC__) && (defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_ARCH_RP2040))
  for (unsigned long mytime = millis () ; mytime + 10000u > millis () ; delay (100)) {
    if (Serial) break ;
  }
#endif
  //
  Signal::setup () ;
  //
  Time::setup (callBacks) ;
  //
  Pressure::setup (callBacks) ;
  //
  Button::setup (callBacks) ;
  //
  Control::setup (callBacks) ;
  //
  Pulser::setup (callBacks) ;
  //
  Relais::setup (callBacks) ;
  //
  Flow::setup (callBacks) ;
  //
  {
    Signal::Hook hook (Signal::scheme::blinkslow) ;
    const unsigned int waitForCmd {4000} ;
    bool enable {__LOG_BY_DEFAULT__} ;
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
        enable = enable ? false : true ;
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
    if (enable) {
      //
      Data::setup (callBacks) ;
      // 
    }
    //
  }
  //
#ifdef __DEBUG__RPAC__
  //
  {
      Debug::setup (callBacks, Serial, 1000, 4) ;
  }
  //
  Serial.println ("[INFO] Setup completed.") ;
  //
#endif
  //
}
//
// the loop function runs over and over again forever
//
void loop () {
  //
#ifdef __DEBUG__RPAC__
  unsigned long loopBegin = millis () ;
#endif
  //
  Control::ctrl_t control = Control::loop (Button::loop (), 4U) ;
  //
  switch (Control::command (control)) {
    //
    case 1 :
      //
      if (Pulser::toggle (Pulser::mode_t::mAuto)) {
        //
        Signal::async (Signal::scheme::blinkfast, 100) ;
        //
      }
      //
      break ;
      //
    case 2 :
      //
      if (Pulser::toggle (Pulser::mode_t::mTune)) {
        //
        Signal::async (Signal::scheme::blinkfast, 200) ;
        //
      }
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
      Data::stop () ;
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
  Logger::loop () ;
  //
#ifdef __DEBUG__RPAC__
  //
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Loop exceeded " + String (loopMaxDura) + " ms.") ;
  //
#endif
  //
  Signal::loop (Pulser::loop (Control::trigger (control))) ;
  //
#ifdef ARDUINO_UBLOX_NINA_W10
  //  Prevent on RTOS operating systems the default watchdog from firing ...
  delay (1) ;
  //
#endif
}
//
#ifdef ARDUINO_SEEED_XIAO_RP2040
//
void setup1 (void) {
  //
  while (!Serial) { }
  //
  Serial.printf("core1: Starting ...\n");
  //
}
//
void loop1 (void) {
  //
  Flow::loop1 () ;
  //
}
//
#endif