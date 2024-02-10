//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
#include "logger.h"
#include "control.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Flow {
        //
        const static uint8_t countsPerUnitSize = 10 ;
        const static uint8_t countsPerUnit [countsPerUnitSize] ;
        //
        static void handler (void) ;
        //
        static volatile long int total ;
        static volatile short int counts ; 
        static bool lowRes ;
        static volatile bool trigger ;
        static uint8_t pos ;
#ifdef __DEBUG__FLOW__
        static volatile bool error ;
#endif
        //
        public :
            static void setup (controlCBs_t &, loggerCBs_t &) ;
            static bool loop (void) ;
    } ;
//
} ;
//
#endif
