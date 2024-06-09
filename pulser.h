//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#ifdef ARDUINO_SEEED_XIAO_RP2040
#ifdef __DEBUG__PULSER__
#define _PWM_LOGLEVEL_        3
#endif
#include "RP2040_PWM.h"
#endif
//
#include "global.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Pulser {
        //
        enum struct Mode : uint8_t { mBase = 0u, mAuto = 1u, mTune = 2u, mDose = 3u} ;
        //
        const static unsigned long __on [] ;
        const static unsigned long __off [] ;
        const static unsigned int __cycles [] ;
        //
        static unsigned long change ;
        static unsigned short stage, cycle ;
        static bool pulse ;
        static Mode mode ;
        //
#ifdef ARDUINO_SEEED_XIAO_RP2040
        static RP2040_PWM * _PWM_Instance ;
        static float _PWM_freq, _PWM_full, _PWM_zero ;
#endif
        //
        inline static void __pulseOn (void) ;
        inline static void __pulseOff (void) ;
        inline static void __nextCycle (void) ;
        //
        Pulser () ;
        //
        public :
            //
            typedef Mode mode_t ;
            //
            static void setup (loggerCBs_t &) ;
            static bool loop (bool) ;
            static bool toggle (mode_t) ;
    } ;
} ;
//
#endif
