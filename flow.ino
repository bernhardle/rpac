//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
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
static volatile long int flowCountsTotal = 0 ;
static volatile short int flowCounts = 0 ; 
static uint8_t flowCountsPerUnitPos = 0 ;
static bool flowCountLowRes = true ;
//
static volatile bool flowTrigger = false ;
static uint8_t flowMeterPin ;
#ifdef __DEBUG__FLOW__
static volatile bool flowTriggerError = false ;
#endif
//
static uint8_t flowControlCB (uint8_t) {
  //
  if (flowCountLowRes) {
    //
    flowCountLowRes = false ;
    //
#ifdef __DEBUG__FLOW__
  Serial.println ("[INFO] flowControlCB () switched to high resolution 1:10") ;
#endif
    return 1U ;
    //
  } else {
    //
    flowCountLowRes = true ;
    //
#ifdef __DEBUG__FLOW__
  Serial.println ("[INFO] flowControlCB () switched to low resolution 1:1") ;
#endif
    return 0U ;
  }
  //
}
//
static void flowIntHandler (void) {
  //
  flowCounts ++ ;
  //
  flowCountsTotal ++ ;
  //
  if (flowCounts == flowCountsPerUnit [flowCountsPerUnitPos]) {
    //
#ifdef __DEBUG__FLOW__
    flowTriggerError = flowTrigger ;  // 'flowTrigger' should be cleared at this time otherwise flag error condition
#endif
    //
    flowTrigger = flowCountLowRes ? flowCountsPerUnitPos == 0 : true ;
    //
    flowCounts = 0 ;
    flowCountsPerUnitPos ++ ; 
    if (flowCountsPerUnitPos == flowCountsPerUnit.size()) flowCountsPerUnitPos = 0 ;
    //
  }
  //
}
//
static unsigned long int flowDataCB (void) {
  //
  return flowCountsTotal ;
  //
}
//
void flowSetup (rpacPin_t pin, controlCBs_t & ccbs, loggerCBs_t & lcbs) {
  //
  pinMode ((flowMeterPin = static_cast <uint8_t> (pin)), INPUT) ;  // Pin is allowed to float as there is a 4.7k pullup in the flow counter
  //
  attachInterrupt(digitalPinToInterrupt(flowMeterPin), flowIntHandler, FALLING) ;
  //
  lcbs.add (& flowDataCB, "flow") ;
  ccbs.add (& flowControlCB, 3) ;
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