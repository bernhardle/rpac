//
#include "logger.h"
#include "control.h"
//
#ifdef __DEBUG_CONTROL__
#define __switchControlMode(a)  (Serial.println ("[Debug] Control " + String (controlMode, DEC) + " -> " + String (a, DEC)), controlMode = a)
#else
#define __switchControlMode(a)  (controlMode = a)
#endif
//
static unsigned long controlButtonTimeHigh = 0, controlButtonTimeLow = 0, controlLastCmd = 0 ;
static uint8_t controlMode = 0, controlCount = 0 ;
//
template <int n> controlCallbacks <n>::controlCallbacks (void) {
  cbs.fill ([](uint8_t val) -> uint8_t { return val ; }) ;
}
//
static unsigned long int controlDataCB (void) {
  //
  auto r = controlLastCmd ;
  controlLastCmd = 0x0UL ;
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
          if (myTime > controlButtonTimeHigh + 350) {
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
        Serial.print ("[Debug] Control command # ") ;
        Serial.println (String (controlCount, DEC)) ;
#endif
        controlLastCmd = (*ccbs [controlCount])(controlCount) << 8 | controlCount ;
        __switchControlMode(0) ;
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