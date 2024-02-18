//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#include "global.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Pulser {
        //
        const static unsigned long __on [] ;
        const static unsigned long __off [] ;
        const static unsigned int __cycles [] ;
        //
        static unsigned long change ;
        static unsigned short mode, cycle ;
        static bool automate, pulse ;
        //
        inline static void __nextCycle (void) ;
        //
        Pulser () ;
        //
        public :
            static void setup (loggerCBs_t &) ;
            static bool loop (bool) ;
            static bool autox (void) ;
            static bool autox (bool) ;
    } ;
} ;
//
#endif
