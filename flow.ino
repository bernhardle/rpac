#include "flow.h"
#include "logger.h"
//
#ifdef ARDUINO_UBLOX_NINA_W10
#include <array>
const std::array<uint8_t,10> flowCountsPerUnit = {{12U,11U,12U,11U,12U,11U,12U,11U,12U,12U}} ;
#else
#include <Array.h>
const Array <uint8_t,10> flowCountsPerUnit = {{12U,11U,12U,11U,12U,11U,12U,11U,12U,12U}} ;
#endif
//
static volatile long int flowLastTick = 0, flowCountsTotal = 0, flowCountsTotalLast = 0, flowCountsTotalLastTime = 0, flowCountMeanTime = 0 ;
static volatile short int flowCounts = 0, flowLastProgress = 0 ; 
static uint8_t flowCountsPerUnitPos = 0 ;
//
static volatile bool flowTrigger = false ;
static pin_size_t flowMeterPin ;
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
  flowLastTick = millis () ;
  //
  if (flowCounts == flowCountsPerUnit [flowCountsPerUnitPos]) {
    //
    flowCounts = 0 ;
    flowCountsPerUnitPos ++ ; 
    if (flowCountsPerUnitPos == flowCountsPerUnit.size()) flowCountsPerUnitPos = 0 ;
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
unsigned long int flowLastActiveTime (void) {
  //
  return flowLastTick ;
  //
}
//
void flowSetup (pin_size_t pin, loggerCBs_t & callbacks) {
  //
  pinMode ((flowMeterPin = pin), INPUT) ;  // Pin is allowed to float as there is a 4.7k pullup in the flow counter
  //
#ifdef __DEBUG__FLOW__
  pinMode (flowGroundPin, OUTPUT) ;
  pinMode (flowMirrorPin, OUTPUT) ;
  //
  digitalWrite (flowGroundPin, LOW) ;
  digitalWrite (flowMirrorPin, HIGH) ;
  delay (300) ;
  digitalWrite (flowMirrorPin, LOW) ;
#endif
  //
  attachInterrupt(digitalPinToInterrupt(flowMeterPin), flowIntHandler, FALLING) ;
  //
  flowLastTick = flowCountsTotalLastTime = millis () ;
  //
  callbacks.add (& flowDataCB, "flow") ;
  //
#ifdef __DEBUG__FLOW__
  {
    
    Serial.print ("[INFO] Flow counter divides by ") ;
    Serial.println ("...") ;
  }
#endif
}
//
bool flowLoop (void) {
  //
  unsigned long int progress = flowCountsTotal - flowCountsTotalLast ;
  unsigned long int now = millis () ;
  unsigned long int interval = now - flowCountsTotalLastTime ;
  //
  if (progress > 0) {
    //
    flowCountMeanTime = interval / progress ;
    //
    flowCountsTotalLast = flowCountsTotal ;
    //
    flowCountsTotalLastTime = now ;
    //
    flowLastProgress = progress ;
    //
  } else {
    //
    flowCountMeanTime = flowLastProgress > 0 ? interval / flowLastProgress : interval ;
    //
  }
  //
  //
#ifdef __DEBUG__FLOW__
  digitalWrite (flowMirrorPin, digitalRead (flowMeterPin)) ;
#endif
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