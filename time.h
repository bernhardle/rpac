//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __time_h_included__
#define __time_h_included__
//
#include "global.h"
//
class DateTime ;
//
namespace rpac {
    //
    template <class T> class Time {
#if defined(ARDUINO_UBLOX_NINA_W10) || defined(ARDUINO_SEEED_XIAO_RP2040)
        static_assert(std::is_base_of<RTC_I2C,T>::value, "[ERROR] RTC driver class must be derived from RTC_I2C") ;
#endif
        static T rtc ;
        static DateTime now ;
        static unsigned long int bias ;
        static char stamp [64] ;
        //
        Time () ;
        //
        public :
            //
            static void setup (loggerCBs_t &) ;
            // inline static const char * start (void) { return stamp ; }
    } ;
    //
} ;
//
#endif