//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "logger.h"
#include "control.h"
//
#ifdef __DEBUG_CONTROL__
#define __switchControlMode(a)  (Serial.println ("[Debug] Control " + String (mode, DEC) + " -> " + String (a, DEC)), mode = a)
#else
#define __switchControlMode(a)  (mode = a)
#endif
//
unsigned long rpac::Control::timeHigh {0} ;
unsigned long rpac::Control::timeLow {0} ;
unsigned long rpac::Control::lastCmd {0} ;
unsigned int rpac::Control::mode {0} ;
unsigned int rpac::Control::count {0} ;
unsigned int rpac::Control::minimal {50} ;
unsigned int rpac::Control::maximal {350} ; 
unsigned int rpac::Control::recover {500} ; 
//
void rpac::Control::setup (loggerCBs_t & lcbs, unsigned int max, unsigned int min, unsigned int rec) {
    //
    mode = 0 ;
    timeHigh = 0 ;
    timeLow = 0 ;
    lcbs.add ([]() -> unsigned long {
      //
      auto r = lastCmd ;
      lastCmd = 0x0UL ;
      return static_cast <unsigned short> (r) ;
      //
    }, String ("Control")) ;
    //
}
//
rpac::Control::ctrl_t rpac::Control::loop (const bool button, unsigned int maxi) {
    //
    unsigned long myTime {millis ()} ;
    //
    switch (mode) {
      //
      case 0 :
        //
        if (button) {
          //
          __switchControlMode(1) ;
          timeHigh = myTime ;
          count = 1 ;
          //
        }
        //
        return static_cast <ctrl_t> (0x00U) ;
        //
      case 1 :
        //
        if (button) {
          //
          if (myTime > timeHigh + maximal) {
            //
            __switchControlMode(4) ;
            //
          }
          //
        } else {
          //
          if (myTime < timeHigh + minimal) {
            //
            __switchControlMode(0) ;
            //
          } else {
            //
            __switchControlMode(2) ;
            //
            timeLow = myTime ;
            //
          }
          //
        }
        //
        return static_cast <ctrl_t> (0x00U) ;
        //
      case 2 :
        //
        if (button) {
          //
          if(count + 1 > maxi) {
            //
            __switchControlMode(4) ;
            //
          } else {
            //
            __switchControlMode(1) ;
            //
            count ++ ;
            timeHigh = myTime ;
            //
          }
          //
        } else {
          //
          if (myTime > timeLow + maximal) {
            //
            __switchControlMode(3) ;
            //
          } 
        }
        //
        return static_cast <ctrl_t> (0x00U) ;
        //
      case 3:
        //
#ifdef __DEBUG_CONTROL__
        Serial.print ("[Debug] Control command # ") ;
        Serial.println (String (count, DEC)) ;
#endif
        lastCmd = count ;
        __switchControlMode(0) ;
        //
        return static_cast <ctrl_t> (button ? count << 1 | 0x01U : count << 1) ;
        //
      case 4:
        //
        if (!button) {
          //
          __switchControlMode(5) ;
          //
          timeLow = myTime ;
          //
          return 0x0 ;
          //
        } else {
          //
          return static_cast <ctrl_t> (count > 1 ? 0x00U : 0x01U) ;
          //
        }
        //
      case 5 :
        //
        if (button) {
          //
          __switchControlMode(4) ;
          //
          return static_cast <ctrl_t> (count > 1 ? 0x00U : 0x01U) ;
          //
        } else if (! button && myTime > timeLow + recover) {
          //
          __switchControlMode(0) ;
          //
          return static_cast <ctrl_t> (0x00U) ;
          //
        }
        //
      default :
        //
        return static_cast <ctrl_t> (0x00U) ;
    }
    //
  }
  //
  #undef __switchControlMode