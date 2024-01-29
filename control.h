#ifndef __control_h_included__
#define __control_h_included__
//
#ifdef ARDUINO_UBLOX_NINA_W10
#include <array>
#else
#include <Array.h>
#endif
//
#include "global.h"
//
template <int n> class controlCallbacks {
    private :
#ifdef ARDUINO_UBLOX_NINA_W10
        std::array <uint8_t (*)(void), n> cbs ;
#else
        Array <uint8_t (*)(void), n> cbs ;
#endif
    public :
        controlCallbacks (void) ;
        inline uint8_t (*operator[](int i) const)(void) {
            return cbs.at(i) ;
        }
        inline void add (uint8_t (*f)(void), int i) { cbs.at(i) = f ; }
        inline int size (void) const { return cbs.size() ; }
} ;
//
typedef controlCallbacks <8> controlCBs_t ;
//
extern void controlSetup (controlCBs_t &, loggerCBs_t &) ;
extern bool controlLoop (bool, const controlCBs_t &) ;
//
#endif