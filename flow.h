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
        static void _handler (void) ;
        //
        static volatile unsigned long total ;
        //
        //      Member variables for high resolution counter (hrc) mode
        //
        const static uint8_t _hrc_countsPerUnitSize {10} ;
        const static uint8_t _hrc_countsPerUnit [_hrc_countsPerUnitSize] ;
        //
        static volatile unsigned short _hrc_lap ;
        static bool _hrc_high ;
        static volatile bool trigger ;
        static unsigned int _hrc_pos ;
        //
        //      Member variables for sliding mean value (smw) calculation
        //
        constexpr static short int _smv_sampleSize {16} ;
        constexpr static short int _smv_hBufSize {_smv_sampleSize + 1} ;
        constexpr static short int _smv_sampleInterval {500} ;      // ms
        const static short int _smv_mult [_smv_sampleSize] ;
        constexpr static long int _smv_div () { long int sum = 0; for (int i = 0; i < _smv_sampleSize ; sum += _smv_mult [i++]) ; return _smv_sampleInterval * sum ; }
        //
        static short int _smv_hBuf [_smv_hBufSize] ;
        static short int _smv_pos ;
        static unsigned long int _smv_posUpd ;
        //
#ifdef __DEBUG__FLOW__
        static volatile bool error ;
#endif
        //
        Flow () ;
        //
        public :
            static void setup (loggerCBs_t &) ;
            static bool loop (void) ;
#ifdef ARDUINO_SEEED_XIAO_RP2040
            static void loop1 (void) ;
#endif
            static bool resox (void) ;
            static unsigned short int mean (void) ;
    } ;
//
} ;
//
#endif
