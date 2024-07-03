//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __softpid_h_included__
#define __softpid_h_included__
//
#include "pressure.h"
#include "logger.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class SoftPID : Pressure <p> {
        constexpr static float pTune {1.2f}, iTune{10.0f}, dTune{0.002f} ;
        static float sVal ;
        static float iVal ;
        static float oVal ;
        static QuickPID pid ;
        SoftPID () {}
        public :
            void setup (loggerCBs_t &) ;
            void loop () ;
    } ;
    //
} ;
//
#endif