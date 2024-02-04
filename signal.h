#ifndef __signal_h_included__
#define __signal_h_included__
//
#include "global.h"
#include "control.h"
//
template <uint8_t n> class signaltrain {
  //
  public :
    enum struct sequence {dark = 0, heartbeat = 1, blinkfast = 2, blinkslow = 3} ;
  private :
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
extern void signalLaunchBlocking (uint8_t) ;
extern void signalLaunchAsync (uint8_t) ;
extern void signalLoop (bool) ;
//
#endif