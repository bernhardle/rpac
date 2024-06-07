//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "signal.h"
//
#ifdef ARDUINO_SEEED_XIAO_RP2040
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels (1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800) ;
#endif
//
template <rpacPin_t p> const uint16_t rpac::Signal <p>::__sigseq [4]{__pack(0U,100U), __pack (100U,0U), __pack(50U, 50U), __pack (255U,255U)} ;
template <rpacPin_t p> unsigned long rpac::Signal <p>::timeOut{0} ;
template <rpacPin_t p> typename rpac::Signal <p>::status rpac::Signal <p>::modus{status::uninitialized} ;
template <rpacPin_t p> typename rpac::Signal <p>::scheme rpac::Signal <p>::pattern{scheme::dark} ;
template <rpacPin_t p> unsigned long rpac::Signal <p>::end{0} ;
template <rpacPin_t p> unsigned long rpac::Signal <p>::counter{0} ; 
template <rpacPin_t p> unsigned long rpac::Signal <p>::head{0} ; 
template <rpacPin_t p> unsigned long rpac::Signal <p>::tail{0} ;
template <rpacPin_t p> bool rpac::Signal <p>::led{false} ;
//
template <rpacPin_t p> void rpac::Signal <p>::__switchLED (bool s) {
    //
    if (led != s) {
      //
#if defined(ARDUINO_UBLOX_NINA_W10) || defined(ARDUINO_SEEED_XIAO_RP2040)
      digitalWrite (static_cast <uint8_t> (p), ! (led = s)) ;
#else
      digitalWrite (static_cast <uint8_t> (p), (led = s)) ;
#endif
      //
#if defined(ARDUINO_SEEED_XIAO_RP2040)
      pixels.setPixelColor (0, led ? pixels.Color (245, 185, 0) : pixels.Color (5, 5, 5)) ;
      pixels.show () ;
      pixels.clear () ;
#endif
      //
    }
    //
  }
//
template <rpacPin_t p> rpac::Signal <p>::Hook::Hook (Signal<p>::scheme s) {
  //
  async (s, 0) ;
  //
}
//
template <rpacPin_t p> rpac::Signal <p>::Hook::~Hook () {
  //
  async (Signal::scheme::dark, 1) ;
  //
}
//
template <rpacPin_t p> void rpac::Signal <p>::setup (void) {
  //
  pinMode (static_cast <uint8_t> (p), OUTPUT) ;
  //
#if defined(ARDUINO_UBLOX_NINA_W10) || defined(ARDUINO_SEEED_XIAO_RP2040)
  digitalWrite (static_cast <uint8_t> (p), ! (led = false)) ;
#else
  digitalWrite (static_cast <uint8_t> (p), (led = false)) ;
#endif
//
#if defined(ARDUINO_SEEED_XIAO_RP2040)
  pixels.begin () ;
  pinMode (NEOPIXEL_POWER, OUTPUT) ;
  digitalWrite (NEOPIXEL_POWER, HIGH) ;
#endif
  //
  modus = status::idle ;
  //
}
//
template <rpacPin_t p> void rpac::Signal <p>::async (scheme schema, unsigned short duration) {
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
  end = duration == 0 ? ULONG_MAX : millis () + duration ;
  pattern = schema ;
  modus = status::start ;
  //
}
//
template <rpacPin_t p> void rpac::Signal <p>::blocking (scheme s, unsigned short n) {
  //
  for (async (s, n) ; modus != status::idle ; loop (false)) ;
  //
}
//
template <rpacPin_t p> bool rpac::Signal <p>::loop (bool ext) {
  //
  unsigned long int myTime = millis () ;
  //
  switch (modus) {
    //
    case status::start :
      //
      head = __head (__sigseq [static_cast <int> (pattern)]) ;
      tail = __tail (__sigseq [static_cast <int> (pattern)]) ;
      counter = 0 ;
      timeOut = myTime + head ;
      modus = status::bright ;
      if (head > 0) __switchLED (true) ;
      break ;
      //
    case status::bright :
      //
      if (myTime > timeOut) {
        //
        timeOut = myTime + tail ;
        modus = status::cycle ;
        if (tail > 0) __switchLED (false) ;
        //
        if (timeOut > end) {
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
        if (head > 0) __switchLED (true) ;
        //
      }
      break ;
      //
    case status::idle :
      //
      __switchLED (ext) ;
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