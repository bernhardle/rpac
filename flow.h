//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
#include "logger.h"
//
#if defined(ARDUINO_Seeed_XIAO_nRF52840)
#include "Adafruit_TinyUSB.h"
#endif
//
namespace rpac {
    //
    template <rpacPin_t p> class Flow {
        //
        static volatile unsigned long total ;
        //
        //      variables for high resolution counter (hrc) mode
        //
        const static uint8_t _hrc_countsPerUnitSize {10} ;
        const static uint8_t _hrc_countsPerUnit [_hrc_countsPerUnitSize] ;
        //
        static volatile unsigned short _hrc_lap ;
        static bool _hrc_high ;
        static volatile bool _hrc_trigger ;
        static unsigned int _hrc_pos ;
#ifdef __DEBUG__FLOW__
        static volatile bool _hrc_error ;
#endif
        //
        //      variables for sliding mean value (smw) calculation
        //
        constexpr static uint16_t _smv_sampleSize {10} ;                       //
        constexpr static uint16_t _smv_hBufSize {_smv_sampleSize + 1} ;        //
        constexpr static uint16_t _smv_sampleInterval {150} ;                  // ms
        // constexpr static short int _smv_mult [16] {22u, 21u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 12u, 11u, 10u, 8u, 6u, 2u} ;
        constexpr static uint16_t _smv_mult [_smv_sampleSize]{22u, 22u, 21u, 20u, 18u, 16u, 14u, 7u, 4u, 1u} ;
        inline static uint32_t _smv_div () { uint32_t sum = 0; for (uint8_t i = 0; i < _smv_sampleSize ; sum += _smv_mult [i++]) ; return _smv_sampleInterval * sum ; }
        //
        static int16_t _smv_hBuf [_smv_hBufSize] ;
        static int16_t _smv_pos ;
        static uint16_t _smv_ret ;
        static uint32_t _smv_posUpd ;
        //
        Flow () ;
        //
        static void _handler (void) ;
        //
        public :
            //
            typedef unsigned short int flow_t ;
            static void setup (loggerCBs_t &) ;
            static flow_t loop (void) ;
#ifdef ARDUINO_SEEED_XIAO_RP2040
            static void loop1 (void) ;
#endif
            static void zero (void) ;
            static bool resox (void) ;
            // inline static unsigned short int mean (void) { return _smv_ret ; }
            inline static bool trigger (flow_t c) { return c & 0x1 ; }
            inline static unsigned short int mean (flow_t c) { return c & 0x1 ; }
    } ;
//
} ;
//
#endif
