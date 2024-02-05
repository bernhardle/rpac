#ifndef __signal_h_included__
#define __signal_h_included__
//
#include "global.h"
#include "control.h"
//
namespace sig {
  //
  enum struct scheme : uint8_t {dark = 0, flat = 1, heartbeat = 2, blinkfast = 3, blinkslow = 4 } ;
  //
} ;
//
class SignalHook {
  //
  private :
    //
  public :
    //
    void operator ()(void) const ;
    SignalHook (sig::scheme) ;
    ~SignalHook () ;
} ;
//
extern void signalSetup (rpacPin_t, controlCBs_t &) ;
extern void signalLaunchAsync (sig::scheme, uint8_t) ;
extern void signalLaunchBlocking (sig::scheme, uint8_t) ;
extern void signalLoop (bool) ;
//
#endif