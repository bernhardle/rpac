//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#ifndef __rtc_h_included__
#define __rtc_h_included__
//
#include "global.h"
//
#include <RTClib.h>
//
class DateTime ;
//
namespace rpac {
    //
    template <class T> class Time {
#if defined(ARDUINO_UBLOX_NINA_W10) || defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_Seeed_XIAO_nRF52840) || defined(ARDUINO_SEEED_XIAO_NRF52840)
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
            static void adjust (DateTime &&) ;
    } ;
    //
} ;
//
#endif