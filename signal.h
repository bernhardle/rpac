#ifndef __signal_h_included__
#define __signal_h_included__
//
#include "global.h"
//
template <uint8_t n> class signaltrain {
  //
  public :
    enum struct sequence {dark = 0, heartbeat = 1, blinkfast = 2, blinkslow = 3} ;
  private :
    // std::array <unsigned short int, n> seq ;
    //
    uint8_t rep = 0, pos = 0 ;
    sequence seq ;
    //
  public :
    signaltrain (sequence a, uint8_t b) : seq (a), rep (b) {}
  //
} ;
//
extern void signalSetup (rpacPin_t, controlCBs_t &) ;
extern void signalLaunch (uint8_t) ;
extern void signalLoop (bool) ;
//
#endif