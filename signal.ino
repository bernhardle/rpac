#include "signal.h"
#include "control.h"
//
namespace {
  //
  enum struct status : uint8_t { start = 0, bright = 1, cycle = 2, idle = 10, uninitialized = UCHAR_MAX} ;
  //
  static uint8_t signalPin, signalRepetitions = 0 ;
  static bool signalState = false ;
  static status signalstatus = status::uninitialized ;
  static sig::scheme signalscheme = sig::scheme::dark ;
  //
  uint8_t signalControlCB (uint8_t cmd) {
    //
#ifdef __DEBUG__SIGNAL__
    Serial.println ("[INFO] signalControlCB () ...") ;
#endif
    signalLaunchAsync (sig::scheme::blinkfast, 3) ;
    //
    return cmd ;
  }
  //
  void signalSwitchLED (bool state) {
    //
    if (state != signalState) {
      //
      signalState = state ;
      //
#ifdef ARDUINO_UBLOX_NINA_W10
      digitalWrite (signalPin, ! signalState) ;
#else
      digitalWrite (signalPin, signalState) ;
#endif
      //
    }
    //
  }
  //
} ;
//
void SignalHook::operator ()(void) const {
  //
  signalLoop (false) ;
  //
}
//
SignalHook::SignalHook (sig::scheme s) {
  //
  signalLaunchAsync (s, UCHAR_MAX) ;
  //
}
//
SignalHook::~SignalHook () {
  //
  signalstatus = status::idle ;
  //
}
//
void signalSetup (rpacPin_t pin, controlCBs_t & ccbs) {
  //
  pinMode ((signalPin = static_cast <uint8_t> (pin)), OUTPUT) ;
  //
  digitalWrite (signalPin, LOW) ;
  //
  signalState = false ;
  //
  ccbs.add (& signalControlCB, 1) ;
  //
  signalstatus = status::idle ;
  //
}
//
void signalLaunchAsync (sig::scheme s, uint8_t i) {
  //
  if (signalstatus == status::uninitialized) {
    //
#ifdef __DEBUG__SIGNAL__
  Serial.println ("[INFO] signalLaunchAsync () called before signalSetup () ...") ;
#endif
    //
    return ;
  }
  //
  signalRepetitions = i ;
  signalscheme = s ;
  signalstatus = status::start ;
  //
}
//
void signalLaunchBlocking (sig::scheme s, uint8_t n) {
  //
  for (signalLaunchAsync (s, n) ; signalstatus != status::idle ; signalLoop (false)) ;
  //
}
//
void signalLoop (bool state) {
  //
  static unsigned long int signalTimeOut = 0 ;
  static uint8_t signalCounter = 0 ;
  //
  unsigned long int myTime = millis () ;
  //
  switch (signalstatus) {
    //
    case status::start :
      //
      signalCounter = 0 ;
      signalTimeOut = myTime + 30 ;
      signalstatus = status::bright ;
      signalSwitchLED (true) ;
      break ;
      //
    case status::bright :
      //
      if (myTime > signalTimeOut) {
        //
        signalTimeOut = myTime + 70 ;
        signalstatus = status::cycle ;
        signalSwitchLED (false) ;
        //
        if (++ signalCounter > signalRepetitions) {
          //
          signalstatus = status::idle ;
          //
        }
        //
      }
      break ;
      //
    case status::cycle :
      //
      if (myTime > signalTimeOut) {
        //
        signalTimeOut = myTime + 30 ;
        signalstatus = status::bright ;
        signalSwitchLED (true) ;
        //
      }
      break ;
      //
    case status::idle :
      //
      signalSwitchLED (state) ;
      //
      break ;
      //
    case status::uninitialized :
      //
      break ;
      //
    default:
      //
      break ;
  }
  //
}
//