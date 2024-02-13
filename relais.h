//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __relais_h_included__
#define __relais_h_included__
//
#include "global.h"
//
namespace rpac {

    template <rpac::rpacPin_t p> class Relais {
        //
        typedef decltype (HIGH) state_t ;
        //
        const static unsigned int __holdDura{100} ;
        //
        static unsigned long lastTime ;
        static bool lastTrigger ;
        static state_t relais ;
        //
        Relais () ;
        //
        public:
            static void setup (loggerCBs_t &) ;
            static bool loop (bool) ;
    } ;
}
//
#endif
