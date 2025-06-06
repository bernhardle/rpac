//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#ifndef __pulser_h_included__
#define __pulser_h_included__
//
#include <String.h>
//
#if (defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_ARCH_RP2040)) && ! defined(ARDUINO_ARCH_MBED)
#ifdef __DEBUG__PULSER__
#define _PWM_LOGLEVEL_        3
#endif
#include "RP2040_PWM.h"
#define __RPAC__RP2040__PWM__
//
#elif defined(ARDUINO_Seeed_XIAO_nRF52840)
#ifdef __DEBUG__PULSER__
#define _PWM_LOGLEVEL_        3
#endif
#include "nRF52_PWM.h"
#define __RPAC__NRF52__PWM__
//
#elif defined(ARDUINO_SEEED_XIAO_NRF52840)
#warning "nRF52840 MBED PWM"
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_MBED_PWM.h"      //https://github.com/khoih-prog/nRF52_MBED_PWM
#define __RPAC__NRF52__MBED__PWM__
//
#elif defined(ARDUINO_ARCH_MBED)
#define __RPAC__MBED__PWM__
#endif
//
#if defined(__RPAC__RP2040__PWM__) || defined(__RPAC__NRF52__PWM__) || defined(__RPAC__MBED__PWM__) || defined(__RPAC__NRF52__MBED__PWM__)
#define __RPAC__ANALOG__PULSE__
#endif
//
#include "global.h"
//
namespace rpac {
    //
    template <rpacPin_t p> class Pulser {
        //
        enum struct Mode : int { mBase = 0u, mAuto = 1u, mTune = 2u, mDose = 3u, mBLE = 4u} ;
        //
        const static uint32_t __on [] ;
        const static uint32_t __off [] ;
        const static uint16_t __cycles [] ;
        //
        static uint32_t change, endTime ;
        static uint16_t stage, cycle ;
        static bool pulse ;
        static Mode mode ;
        //
#if defined(__RPAC__ANALOG__PULSE__)
        static float _PWM_full, _PWM_zero, _PWM_freq ;
#if defined(__RPAC__RP2040__PWM__)
        typedef RP2040_PWM _PWM_instance_t ;
        static _PWM_instance_t * _PWM_Instance ;
#elif defined(__RPAC__NRF52__PWM__)
        typedef nRF52_PWM _PWM_instance_t ;
        static _PWM_instance_t * _PWM_Instance ;
#elif defined(__RPAC__NRF52__MBED__PWM__)
        typedef mbed::PwmOut _PWM_instance_t ;
        static _PWM_instance_t * _PWM_Instance ;
#endif
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
            static constexpr uint16_t maxRemotePulseDurationMS {0x1000} ;        // ~ 4 sec.
            //
            typedef Mode mode_t ;
            //
            static void setup (loggerCBs_t &) ;
            static bool loop (bool) ;
            static bool toggle (mode_t) ;
            //
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
            static void remoteOperation (bool) ;
            static bool remotePulse (uint16_t) ;
            static float remoteDuty (float) ;
#endif
            //
    } ;
} ;
//
#endif
