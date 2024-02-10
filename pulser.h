//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#include "global.h"
#include "control.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Pulser {
        //
        const static unsigned long __on [] ;
        const static unsigned long __off [] ;
        const static unsigned int __cycles [] ;
        //
        static unsigned long int change ;
        static unsigned short int mode, cycle ;
        static bool automate, pulse ;
        //
        public :
            static void setup (controlCBs_t &, loggerCBs_t &) ;
            static bool loop (bool) ;
    } ;
} ;
//
#endif
