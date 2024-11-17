//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __flow_h_included__
#define __flow_h_included__
//
#include "global.h"
#include "logger.h"
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
        constexpr static short int _smv_sampleSize {16} ;
        constexpr static short int _smv_hBufSize {_smv_sampleSize + 1} ;
        constexpr static short int _smv_sampleInterval {500} ;      // ms
        const static short int _smv_mult [_smv_sampleSize] ;
        inline static long int _smv_div () { long int sum = 0; for (int i = 0; i < _smv_sampleSize ; sum += _smv_mult [i++]) ; return _smv_sampleInterval * sum ; }
        //
        static short int _smv_hBuf [_smv_hBufSize] ;
        static short int _smv_pos ;
        static unsigned short int _smv_ret ;
        static unsigned long int _smv_posUpd ;
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
            static bool resox (void) ;
            // inline static unsigned short int mean (void) { return _smv_ret ; }
            inline static bool trigger (flow_t c) { return c & 0x1 ; }
            inline static unsigned short int mean (flow_t c) { return c & 0x1 ; }
    } ;
//
} ;
//
#endif
