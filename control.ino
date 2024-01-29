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
static uint8_t controlMode = 0, controlCount = 0, controlLastCmd = 0 ;
//
template <int n> controlCallbacks <n>::controlCallbacks (void) {
  cbs.fill ([](void) -> uint8_t { return 0U ; }) ;
}
//
static unsigned long int controlDataCB (void) {
  //
  uint8_t r = controlLastCmd ;
  controlLastCmd = 0 ;
  return r ;
  //
}
//
void controlSetup (controlCBs_t & ccbs, loggerCBs_t & lcbs) {
    //
    controlMode = 0 ;
    controlButtonTimeHigh = 0 ;
    controlButtonTimeLow = 0 ;
    lcbs.add (& controlDataCB, "control") ;
    //
}
//
bool controlLoop (bool button, const controlCBs_t & ccbs) {
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
          controlCount = 1 ;
          button = false ;
          //
        }
        //
        break ;
        //
      case 1 :
        //
        if (button) {
          //
          if (myTime > controlButtonTimeHigh + 400) {
            //
            __switchControlMode(4) ;
            //
          }
          //
          button = false ;
          //
        } else {
          //
          if (myTime < controlButtonTimeHigh + 50) {
            //
            __switchControlMode(0) ;
            //
          } else {
            //
            __switchControlMode(2) ;
            controlButtonTimeLow = myTime ;
            //
          }
          //
        }
        //
        break ;
        //
      case 2 :
        //
        if (button) {
          //
          if(controlCount + 2 > ccbs.size ()) {
            //
            __switchControlMode(4) ;
            button = false ;
            //
          } else {
            //
            __switchControlMode(1) ;
            button = false ;
            controlCount ++ ;
            controlButtonTimeHigh = myTime ;
            //
          }
          //
        } else {
          //
          if (myTime > controlButtonTimeLow + 500) {
            //
            __switchControlMode(3) ;
            //
          } 
        }
        break ;
        //
      case 3:
        //
#ifdef __DEBUG_CONTROL__
        Serial.print ("[Debug] Control # ") ;
        Serial.println (String (controlCount, DEC)) ;
#endif
        (*ccbs [controlCount])() ;
        __switchControlMode(0) ;
        controlLastCmd = controlCount ;
        //
        break ;
        //
      case 4:
        //
        if (!button) {
          //
          __switchControlMode(5) ;
          controlButtonTimeLow = myTime ;
          //
        } else {
          //
          if (controlCount > 1) button = false ;
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
          if (controlCount > 1) button = false ;
          //
        } else if (! button && myTime > controlButtonTimeLow + 500) {
          //
          __switchControlMode(0) ;
          //
        }
        break ;
        //
      default :
        //
        break ;
    }
    //
    return button ;
    //
  }
  //
  #undef __switchControlMode