//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __control_h_included__
#define __control_h_included__
//
#include "global.h"
//
namespace rpac {
    //
    template <rpacPin_t b> class Control {
        //
        //  template params:
        //  b ... button pin (pro forma)
        //
        //  __mint ... trigger minimum separation [ms]
        //  __maxt ... trigger maximum separation [ms]
        //  __reco ... trigger recover time [ms]
        //
        static unsigned long timeHigh, timeLow, lastCmd ;
        static unsigned int mode, count, minimal, maximal, recover ;
        //
        Control () ;
        //
        public :
            //
            typedef uint8_t ctrl_t ;
            //
            static void setup (loggerCBs_t &, unsigned int = 400, unsigned int = 50, unsigned int = 700) ;
            static ctrl_t loop (bool, unsigned int) ;
            //
            inline static bool trigger (ctrl_t c) { return c & 0x1 ; }
            inline static int command (ctrl_t c) { return c >> 1 ; }
    } ;
    //
} ;
//
#endif