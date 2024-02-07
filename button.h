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
    class SimpleButton {
        //
        public :
            virtual bool pressed (void) = 0 ;
    } ;
    //
    template <rpacPin_t p> class DebouncedButton : public SimpleButton {
        //
        const static uint8_t pins = 32U ;
        //
        static Array <unsigned long int, pins> times ;
        static Array <unsigned short int, pins> counts ;
        //
        volatile unsigned long int & time{times.at(static_cast <int> (p))} ;
        unsigned long int last{0} ;
        unsigned short int & count{counts.at(static_cast <int> (p))} ;
        //
#ifdef __DEBUG__BUTTON__
        bool check{false} ;
#endif
        //
        static inline bool BUTTONSTATE(const unsigned short int & i) { return i > 7 ? true : false ; }
        //
        static void intHandler (void) ;
        static unsigned long dataCB (void) ;
        //
        public :
            void setup (loggerCBs_t &) ;
            bool loop (void) ;
            bool pressed (void) ;
        //
    } ;
    //
}
//
#endif