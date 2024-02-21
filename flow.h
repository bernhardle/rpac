//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
#include "logger.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Flow {
        //
        const static uint8_t countsPerUnitSize {10} ;
        const static uint8_t countsPerUnit [countsPerUnitSize] ;
        //
        static void handler (void) ;
        //
        static volatile unsigned long total ;
        static volatile unsigned short counts ;
        static bool lowRes ;
        static volatile bool trigger ;
        static unsigned int pos ;
#ifdef __DEBUG__FLOW__
        static volatile bool error ;
#endif
        //
        Flow () ;
        //
        public :
            static void setup (loggerCBs_t &) ;
            static bool loop (void) ;
            static bool resox (void) ;
    } ;
//
} ;
//
#endif
