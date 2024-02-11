//
//  (c) Bernhard Schupp, Frankfurt (2024)
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
template <rpac::rpacPin_t b, rpac::rpacPin_t s> unsigned long int rpac::Control <b, s>::controlButtonTimeHigh{0} ;
template <rpac::rpacPin_t b, rpac::rpacPin_t s> unsigned long int rpac::Control <b, s>::controlButtonTimeLow{0} ; 
template <rpac::rpacPin_t b, rpac::rpacPin_t s> unsigned long int rpac::Control <b, s>::controlLastCmd{0} ;
template <rpac::rpacPin_t b, rpac::rpacPin_t s> uint8_t rpac::Control <b, s>::controlMode{0} ;
template <rpac::rpacPin_t b, rpac::rpacPin_t s> uint8_t rpac::Control <b, s>::controlCount{0} ;
//
template <int n> rpac::controlCallbacks <n>::controlCallbacks (void) {
  //
  cbs.fill ([](uint8_t val) -> uint8_t { return val ; }) ;
  //
}
//
template <rpac::rpacPin_t b, rpac::rpacPin_t s> void rpac::Control <b, s>::setup (loggerCBs_t & lcbs) {
    //
    controlMode = 0 ;
    controlButtonTimeHigh = 0 ;
    controlButtonTimeLow = 0 ;
    lcbs.add ([]() -> unsigned long {
      //
      auto r = controlLastCmd ;
      controlLastCmd = 0x0UL ;
      return r ;
      //
    }, String ("Control PIN") + String (static_cast <int> (b), DEC)) ;
    //
}
//
template <rpac::rpacPin_t b, rpac::rpacPin_t s> bool rpac::Control <b, s>::loop (bool button, const rpac::controlCBs_t & ccbs) {
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
        Serial.print ("[Debug] Control command # ") ;
        Serial.println (String (controlCount, DEC)) ;
#endif
        controlLastCmd = ccbs [controlCount](controlCount) << 8 | controlCount ;
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