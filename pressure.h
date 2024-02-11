//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __pressure_h_included__
#define __pressure_h_included__
//
#include <String.h>
//
#include "global.h"
//
namespace rpac {
    //
    template <rpacPin_t> class Pressure {
        //
        Pressure () ;
        //
        public :
            static void setup (loggerCBs_t &) ;
            static bool loop (void) ;
    } ;
    //
} ;
//
#endif