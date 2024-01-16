#include "flow.h"
//
static volatile short int flowCounts = 0 ;
static volatile bool flowTrigger = false ;
#ifdef __DEBUG__FLOW__
static volatile bool flowTriggerError = false ;
#endif
//
static void flowIntHandler (void) {
  //
  flowCounts ++ ;
  //
  if (flowCounts == flowCountsPerUnit) {
    //
    flowCounts = 0 ;
    //
#ifdef __DEBUG__FLOW__
    flowTriggerError = flowTrigger ;  // 'flowTrigger' should be cleared at this time otherwise flag error condition
#endif
    //
    flowTrigger = true ;
    //
  }
  //
}
//
void flowSetup (void) {
  //
  pinMode (flowPin, INPUT) ;  // Pin is allowed to float as there is a 4.7k pullup in the flow counter
  //
  attachInterrupt(digitalPinToInterrupt(flowPin), flowIntHandler, FALLING) ;
  //
}
//
bool flowLoop (void) {
  //
  if (flowTrigger) {
    //
    flowTrigger = false ;
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
  if (flowTriggerError) {
    //
    Serial.println ("[WARNING] Flow trigger error flag set.") ;
    //
  }
#endif
  //
  return false ;
  //
}