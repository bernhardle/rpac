//
#include "control.h"
//
#ifdef __DEBUG_CONTROL__
#define __switchControlMode(a)  (Serial.println ("[Debug] Control " + String (controlMode, DEC) + " -> " + String (a, DEC)), controlMode = a)
#else
#define __switchControlMode(a)  (controlMode = a)
#endif
//
static unsigned long controlButtonTimeHigh = 0, controlButtonTimeLow = 0 ;
static uint8_t controlMode = 0, controlCount = 0 ;
//
void controlSetup (void) {
    //
    controlMode = 0 ;
    controlButtonTimeHigh = 0 ;
    controlButtonTimeLow = 0 ;
    //
}
//
uint8_t controlLoop (bool button) {
    //
    unsigned long myTime = millis () ;
    //
    switch (controlMode) {
        //
        case 0 :
          //
          if (button) {
            //
            __switchControlMode(1) ;
            controlButtonTimeHigh = myTime ;
            //
          }
          //
          break ;
          //
        case 1 :
          //
          if (button && myTime > controlButtonTimeHigh + 500) {
            //
            __switchControlMode(4) ;
            //
          } else if (!button && myTime < controlButtonTimeHigh + 50) {
            //
            __switchControlMode(0) ;
            controlCount = 0 ;
            controlButtonTimeLow = 0 ;
            controlButtonTimeHigh = 0 ;
            //
          } else if (!button && myTime > controlButtonTimeHigh + 50) {
            //
            __switchControlMode(2) ;
            controlButtonTimeLow = myTime ;
            //
          }
          //
          break ;
          //
        case 2 :
          //
          if (! button && myTime > controlButtonTimeLow + 500) {
            //
            __switchControlMode(3) ;
            controlCount ++ ;
            //
          } else if (button && myTime < controlButtonTimeLow + 50) {
            //
            __switchControlMode(4) ;
            //
          } else if (button && myTime > controlButtonTimeLow + 50) {
            //
            __switchControlMode(1) ;
            controlCount ++ ;
            controlButtonTimeHigh = myTime ;
            //
          }
          break ;
          //
        case 3:
          //
          {
            int ret = controlCount ;
            Serial.print ("[Debug] Control # ") ;
            Serial.println (String (controlCount, DEC)) ;
            __switchControlMode(0) ;
            controlCount = 0 ;
            controlButtonTimeLow = 0 ;
            controlButtonTimeHigh = 0 ;
            return ret ;
          }
          //
        case 4:
          //
          if (!button) {
            //
            __switchControlMode(5) ;
            controlButtonTimeLow = myTime ;
            //
          }
          break ;
          //
        case 5 :
          //
          if (button) {
            //
            __switchControlMode(4) ;
            //
          } else if (! button && myTime > controlButtonTimeLow + 500) {
            //
            __switchControlMode(0) ;
            controlCount = 0 ;
            controlButtonTimeLow = 0 ;
            controlButtonTimeHigh = 0 ;
            //
          }
          break ;
          //
        default :
          //
          break ;
    }
    //
    return 0 ;
    //
  }
  //
  #undef __switchControlMode