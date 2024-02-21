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
template <rpacPin_t b> unsigned long rpac::Control <b>::timeHigh{0} ;
template <rpacPin_t b> unsigned long rpac::Control <b>::timeLow{0} ;
template <rpacPin_t b> unsigned long rpac::Control <b>::lastCmd{0} ;
template <rpacPin_t b> unsigned int rpac::Control <b>::mode{0} ;
template <rpacPin_t b> unsigned int rpac::Control <b>::count{0} ;
template <rpacPin_t b> unsigned int rpac::Control <b>::minimal{50} ;
template <rpacPin_t b> unsigned int rpac::Control <b>::maximal{350} ; 
template <rpacPin_t b> unsigned int rpac::Control <b>::recover{500} ; 
//
template <rpacPin_t b> void rpac::Control <b>::setup (loggerCBs_t & lcbs, unsigned int max, unsigned int min, unsigned int rec) {
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
    }, String ("Control PIN") + String (static_cast <int> (b), DEC)) ;
    //
}
//
template <rpacPin_t b> typename rpac::Control<b>::ctrl_t rpac::Control <b>::loop (const bool button, unsigned int maxi) {
    //
    unsigned long myTime = millis () ;
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