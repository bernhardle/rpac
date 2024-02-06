//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __signal_h_included__
#define __signal_h_included__
//
#include <limits.h>
//
#include "global.h"
#include "control.h"
//
namespace sig {
  //
  enum struct scheme : uint8_t {dark = 0U, flat = 1U, blinkfast = 2U, blinkslow = 3U } ;
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