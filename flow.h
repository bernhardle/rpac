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
        const static uint8_t _smv_size {16} ;
        const static uint8_t _smv_mult [_smv_size] ;
        const static unsigned long int _smv_interval {250} ;
        //
        static uint8_t _smv_hBuf [_smv_size + 1] ;
        static uint8_t _smv_pos ;
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
