//
#include "control.h"
//
static unsigned long controlButtonTimeHigh = 0, controlButtonTimeLow = 0 ;
static uint8_t controlMode = 0 ;
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
            Serial.println ("[Debug] Control 0 -> 1") ;
            controlMode = 1 ;
            controlButtonTimeHigh = myTime ;
            //
          }
          //
          break ;
          //
        case 1 :
          //
          if (button && myTime > controlButtonTimeHigh + 500 || !button && myTime < controlButtonTimeHigh + 50) {
            //
            Serial.println ("[Debug] Control 1 -> 0") ;
            controlMode = 0 ;
            //
          } else if (!button && myTime > controlButtonTimeHigh + 50) {
            //
            Serial.println ("[Debug] Control 1 -> 2") ;
            controlMode = 2 ;
            controlButtonTimeLow = myTime ;
            //
          }
          //
          break ;
          //
        case 2 :
          //
          if (! button && myTime > controlButtonTimeLow + 500 || button && myTime < controlButtonTimeLow + 50) {
            //
            Serial.println ("[Debug] Control 2 -> 0") ;
            controlMode = 0 ;
            //
          } else if (button && myTime > controlButtonTimeLow + 50) {
            //
            Serial.println ("[Debug] Control 2 -> 1") ;
            controlMode = 1 ;
            controlButtonTimeHigh = myTime ;
            //
          }
          break ;
          //
        default :
          //
          break ;
    }
    //
    return controlMode ;
    //
  }