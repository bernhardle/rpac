#include "flow.h"
//
static volatile long int flowCountsTotal = 0, flowCountsTotalLast = 0, flowCountsTotalLastTime = 0, flowCountMeanTime = 0 ;
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
  flowCountsTotal ++ ;
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
static unsigned long int flowDataCB (void) {
  //
  return flowCountMeanTime ;
  //
}
//
void flowSetup (loggerCBs_t & callbacks) {
  //
  pinMode (flowPin, INPUT) ;  // Pin is allowed to float as there is a 4.7k pullup in the flow counter
  //
  attachInterrupt(digitalPinToInterrupt(flowPin), flowIntHandler, FALLING) ;
  //
  flowCountsTotalLastTime = millis () ;
  //
  callbacks.add (& flowDataCB) ;
  //
}
//
bool flowLoop (void) {
  //
  unsigned long progress = flowCountsTotal - flowCountsTotalLast ;
  //
  if (progress > 0) {
    //
    unsigned long int now = millis () ;
    unsigned long int interval = now - flowCountsTotalLastTime ;
    //
    flowCountMeanTime = interval / progress ;
    //
    flowCountsTotalLast = flowCountsTotal ;
    //
    flowCountsTotalLastTime = now ;
    //
  }
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