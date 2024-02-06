//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifdef ARDUINO_UBLOX_NINA_W10
#include <array>
template <typename A, int n> using Array = std::array <A, n> ;
#else
#include <Array.h>
#endif
//
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
  constexpr uint16_t pack (uint8_t a, uint8_t b) { return a << 8 | b ; }
  constexpr uint8_t head (uint16_t a) { return a >> 8 ; }
  constexpr uint8_t tail (uint16_t a) { return static_cast <uint8_t> (a) ; }
  //
  const Array <uint16_t, 4> sigseq = {{pack(0U,100U), pack (100U,0U), pack(20U, 20U), pack (255U,255U)}} ;
  //
  uint8_t signalControlCB (uint8_t cmd) {
    //
    static uint8_t cnt = 0 ;
    //
#ifdef __DEBUG__SIGNAL__
    Serial.println ("[INFO] signalControlCB () ...") ;
#endif
    signalLaunchAsync (static_cast <sig::scheme> (cnt++ % 4), 10) ;
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
  static uint8_t signalCounter = 0, signalHead = 0, signalTail = 0 ;
  //
  unsigned long int myTime = millis () ;
  //
  switch (signalstatus) {
    //
    case status::start :
      //
      signalHead = head (sigseq [static_cast <uint8_t> (signalscheme)]) ;
      signalTail = tail (sigseq [static_cast <uint8_t> (signalscheme)]) ;
      signalCounter = 0 ;
      signalTimeOut = myTime + signalHead ;
      signalstatus = status::bright ;
      if (signalHead > 0) signalSwitchLED (true) ;
      break ;
      //
    case status::bright :
      //
      if (myTime > signalTimeOut) {
        //
        signalTimeOut = myTime + signalTail ;
        signalstatus = status::cycle ;
        if (signalTail > 0) signalSwitchLED (false) ;
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
        signalTimeOut = myTime + signalHead ;
        signalstatus = status::bright ;
        if (signalHead > 0) signalSwitchLED (true) ;
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