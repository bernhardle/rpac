//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "signal.h"
#include "control.h"
//
template <rpac::rpacPin_t p> const uint16_t rpac::Signal <p>::__sigseq [4]{__pack(0U,100U), __pack (100U,0U), __pack(20U, 20U), __pack (255U,255U)} ;
template <rpac::rpacPin_t p> unsigned long int rpac::Signal <p>::timeOut{0} ;
template <rpac::rpacPin_t p> typename rpac::Signal <p>::status rpac::Signal <p>::modus{status::uninitialized} ;
template <rpac::rpacPin_t p> typename rpac::Signal <p>::scheme rpac::Signal <p>::pattern{scheme::dark} ;
template <rpac::rpacPin_t p> uint8_t rpac::Signal <p>::cycles{0} ;
template <rpac::rpacPin_t p> uint8_t rpac::Signal <p>::counter{0} ; 
template <rpac::rpacPin_t p> uint8_t rpac::Signal <p>::head{0} ; 
template <rpac::rpacPin_t p> uint8_t rpac::Signal <p>::tail{0} ;
template <rpac::rpacPin_t p> bool rpac::Signal <p>::led{false} ;
//
template <rpac::rpacPin_t p> void rpac::Signal <p>::switchLED (bool s) {
    //
    if (led != s) {
      //
#ifdef ARDUINO_UBLOX_NINA_W10
      digitalWrite (static_cast <uint8_t> (p), ! (led = s)) ;
#else
      digitalWrite (static_cast <uint8_t> (p), (led = s)) ;
#endif
      //
    }
    //
  }
//
template <rpac::rpacPin_t p> rpac::Signal <p>::Hook::Hook (Signal<p>::scheme s) {
  //
  async (s, UCHAR_MAX) ;
  //
}
//
template <rpac::rpacPin_t p> rpac::Signal <p>::Hook::~Hook () {
  //
  async (Signal::scheme::dark, 1) ;
  //
}
//
template <rpac::rpacPin_t p> void rpac::Signal <p>::setup (controlCBs_t & ccbs) {
  //
  pinMode (static_cast <uint8_t> (p), OUTPUT) ;
  //
#ifdef ARDUINO_UBLOX_NINA_W10
  digitalWrite (static_cast <uint8_t> (p), ! (led = false)) ;
#else
  digitalWrite (static_cast <uint8_t> (p), (led = false)) ;
#endif
  //
  ccbs.add ([](uint8_t cmd) -> uint8_t {
    //
    static uint8_t cnt = 0 ;
    //
#ifdef __DEBUG__SIGNAL__
    Serial.println ("[INFO] signalControlCB () ...") ;
#endif
    async (static_cast <Signal::scheme> (cnt++ % 4), 10) ;
    //
    return cmd ;
    //
  }, 1) ;
  //
  modus = status::idle ;
  //
}
//
template <rpac::rpacPin_t p> void rpac::Signal <p>::async (scheme s, uint8_t i) {
  //
  if (modus == status::uninitialized) {
    //
#ifdef __DEBUG__SIGNAL__
  Serial.println ("[INFO] signalLaunchAsync () called before signalSetup () ...") ;
#endif
    //
    return ;
  }
  //
  cycles = i ;
  pattern = s ;
  modus = status::start ;
  //
}
//
template <rpac::rpacPin_t p> void rpac::Signal <p>::blocking (scheme s, uint8_t n) {
  //
  for (async (s, n) ; modus != status::idle ; loop (false)) ;
  //
}
//
template <rpac::rpacPin_t p> bool rpac::Signal <p>::loop (bool ext) {
  //
  unsigned long int myTime = millis () ;
  //
  switch (modus) {
    //
    case status::start :
      //
      head = __head (__sigseq [static_cast <uint8_t> (pattern)]) ;
      tail = __tail (__sigseq [static_cast <uint8_t> (pattern)]) ;
      counter = 0 ;
      timeOut = myTime + head ;
      modus = status::bright ;
      if (head > 0) switchLED (true) ;
      break ;
      //
    case status::bright :
      //
      if (myTime > timeOut) {
        //
        timeOut = myTime + tail ;
        modus = status::cycle ;
        if (tail > 0) switchLED (false) ;
        //
        if (++ counter > cycles) {
          //
          modus = status::idle ;
          //
        }
        //
      }
      break ;
      //
    case status::cycle :
      //
      if (myTime > timeOut) {
        //
        timeOut = myTime + head ;
        modus = status::bright ;
        if (head > 0) switchLED (true) ;
        //
      }
      break ;
      //
    case status::idle :
      //
      switchLED (ext) ;
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
  return (modus == status::idle) ;
  //
}
//