//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "flow.h"
#include "logger.h"
//
template <rpacPin_t p> const uint8_t rpac::Flow <p>::countsPerUnit [10]{12U,11U,12U,11U,12U,11U,12U,11U,12U,12U} ;
template <rpacPin_t p> volatile unsigned long rpac::Flow <p>::total{0} ;
template <rpacPin_t p> volatile unsigned short rpac::Flow <p>::counts{0} ; 
template <rpacPin_t p> bool rpac::Flow <p>::lowRes{false} ;
template <rpacPin_t p> volatile bool rpac::Flow <p>::trigger{false} ;
template <rpacPin_t p> unsigned int rpac::Flow <p>::pos{0} ;
#ifdef __DEBUG__FLOW__
template <rpacPin_t p> volatile bool rpac::Flow <p>::error{false} ;
#endif
//
template <rpacPin_t p> void rpac::Flow <p>::handler (void) {
  //
  counts ++ ;
  //
  total ++ ;
  //
  if (counts == countsPerUnit [pos]) {
    //
#ifdef __DEBUG__FLOW__
    error = trigger ;  // 'trigger' should be cleared at this time otherwise flag error condition
#endif
    //
    trigger = lowRes ? pos == 0 : true ;
    //
    counts = 0 ;
    pos ++ ; 
    if (pos == countsPerUnitSize) pos = 0 ;
    //
  }
  //
}
//
template <rpacPin_t p> uint8_t rpac::Flow <p>::resox (void) {
  //
  if (lowRes) {
    //
    lowRes = false ;
    //
#ifdef __DEBUG__FLOW__
    Serial.println ("[INFO] flowControlCB () switched to high resolution 1:10") ;
#endif
    return 1U ;
    //
  } else {
    //
    lowRes = true ;
    //
#ifdef __DEBUG__FLOW__
    Serial.println ("[INFO] flowControlCB () switched to low resolution 1:1") ;
#endif
    return 0U ;
    //
  }
  //
}
//
template <rpacPin_t p> void rpac::Flow <p>::setup (loggerCBs_t & lcbs) {
  //
  pinMode (static_cast <uint8_t> (p), INPUT) ;  // Pin is allowed to float as there is a 4.7k pullup in the flow counter
  //
  attachInterrupt(digitalPinToInterrupt(static_cast <uint8_t> (p)), & handler, FALLING) ;
  //
  lcbs.add ([]() -> unsigned long { return total ; }, "Flow PIN" + String (static_cast <int> (p), DEC)) ;
  //
}
//
template <rpacPin_t p> bool rpac::Flow <p>::loop (void) {
  //
  if (trigger) {
    //
    trigger = false ;
    //
#ifdef __DEBUG__FLOW__
    Serial.println ("[INFO] Flow meter pulsed.") ;
#endif
    //
    return true ;
    //
  }
  //
#ifdef __DEBUG__FLOW__
  if (error) {
    //
    Serial.println ("[WARNING] Flow trigger error flag set.") ;
    //
  }
#endif
  //
  return false ;
  //
}