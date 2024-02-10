//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __time_h_included__
#define __time_h_included__
//
#include "global.h"
#include "control.h"
//
class DateTime ;
//
namespace rpac {
    //
    template <class T> class Time {
        //
        static T rtc ;
        static DateTime now ;
        static unsigned long int bias ;
        static char stamp [64] ;
        //
        public :
            //
            static void setup (loggerCBs_t &) ;
            inline static const char * start (void) { return stamp ; }
    } ;
    //
} ;
//
#endif