//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __control_h_included__
#define __control_h_included__
//
#ifdef ARDUINO_UBLOX_NINA_W10
#include <array>
template <typename A, int n> using Array = std::array <A, n> ;
#else
#include <Array.h>
#endif
//
#include "global.h"
//
template <int n> class controlCallbacks {
    private :
        Array <uint8_t (*)(uint8_t), n> cbs ;
    public :
        controlCallbacks (void) ;
        inline uint8_t (*operator[](int i) const)(uint8_t) {
            return cbs.at(i) ;
        }
        inline void add (uint8_t (*f)(uint8_t), int i) { cbs.at(i) = f ; }
        inline int size (void) const { return cbs.size() ; }
} ;
//
typedef controlCallbacks <8> controlCBs_t ;
//
extern void controlSetup (controlCBs_t &, loggerCBs_t &) ;
extern bool controlLoop (bool, const controlCBs_t &) ;
//
#endif