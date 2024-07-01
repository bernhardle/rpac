//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __button_h_included__
#define __button_h_included__
//
#include "global.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Button {
        //
        static volatile unsigned long time ;
        static unsigned long last ;
        static signed short count ;
        //
#ifdef __DEBUG__BUTTON__
        static bool check ;
#endif
        //
        static inline bool BUTTONSTATE(const unsigned short & i) { return i > 7 ? true : false ; }
        //
        static void _handler (void) ;
        //
        public :
            static void setup (loggerCBs_t &) ;
            static bool loop (void) ;
        //
    } ;
    //
}
//
#endif