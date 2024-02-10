//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __control_h_included__
#define __control_h_included__
//
// #include <forward_list>
#include "global.h"
//
namespace rpac {
    //
    template <rpac::rpacPin_t b, rpac::rpacPin_t s> class Control {
        //
        //  template params:
        //  b ... button pin
        //  s ... signal pin
        //
        static unsigned long controlButtonTimeHigh, controlButtonTimeLow, controlLastCmd ;
        static uint8_t controlMode, controlCount ;
        //
        static unsigned long int data (void) ;
        //
        public :
            static void setup (loggerCBs_t &) ;
            static bool loop (const rpac::controlCBs_t &) ;
    } ;
    //
} ;
//
#endif