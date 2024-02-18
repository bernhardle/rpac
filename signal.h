//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __signal_h_included__
#define __signal_h_included__
//
#include <limits.h>
//
#include "global.h"
#include "logger.h"
//
namespace rpac {
  //
  template <rpacPin_t p> class Signal {
    //
    protected :
      //
      static constexpr uint16_t __pack (uint8_t a, uint8_t b) { return a << 8 | b ; }
      static constexpr uint8_t __head (uint16_t a) { return a >> 8 ; }
      static constexpr uint8_t __tail (uint16_t a) { return static_cast <uint8_t> (a) ; }
      //
    protected :
      //
      enum struct status : uint8_t { start = 0, bright = 1, cycle = 2, idle = 10, uninitialized = UCHAR_MAX} ;
      //
      const static uint16_t __sigseq [] ;
      //
    public :
      //
      enum struct scheme : uint8_t {dark = 0U, flat = 1U, blinkfast = 2U, blinkslow = 3U } ;
      //
      class Hook final {
        //
        scheme pattern{Signal::scheme::dark} ;
        //
        public :
          //
          Hook (scheme) ;
          void * operator new (size_t) = delete ;
          ~Hook () ;
      } ;
      //
    private :
      //
      static void switchLED (bool) ;
      //
      static unsigned long end, timeOut, counter, head, tail ;
      static status modus ;
      static scheme pattern ;
      static bool led ;
      //
      Signal () ;
      //
    public :
      //
      static void setup (void) ;
      static void async (scheme, unsigned short) ;
      static void blocking (scheme, unsigned short) ;
      static bool loop (bool) ;
      //
    } ;
  //
} ;
//
#endif