#include "signal.h"
#include "control.h"
//
static uint8_t signalPin, signalMode = 10, signalRepetitions = 0 ;
static bool signalState = false ;
//
static uint8_t signalControlCB (void) {
  //
#ifdef __DEBUG__SIGNAL__
  Serial.println ("[INFO] signalControlCB () ...") ;
#endif
  signalLaunch (5) ;
  //
  return 0U ;
}
//
static void signalSwitchLED (bool state) {
  //
  if (state != signalState) {
    //
    signalState = state ;
    //
  #ifdef ARDUINO_UBLOX_NINA_W10
    digitalWrite (signalPin, ! signalState) ;
  #else
    digitalWrite (signalPin, signalState) ;
  #endif
    //
  }
}
//
void signalSetup (rpacPin_t pin, controlCBs_t & ccbs) {
  //
  pinMode ((signalPin = static_cast <uint8_t> (pin)), OUTPUT) ;
  //
  digitalWrite (signalPin, LOW) ;
  //
  signalState = false ;
  //
  ccbs.add (& signalControlCB, 1) ;
  //
}
//
void signalLaunch (uint8_t i) {
  //
  signalRepetitions = i ;
  signalMode = 0 ;
  //
}
//
void signalLoop (bool state) {
  //
  static unsigned long int signalTimeOut = 0 ;
  static uint8_t signalCounter = 0 ;
  //
  unsigned long int myTime = millis () ;
  //
  switch (signalMode) {
    //
    case 0 :
      //
      signalCounter = 0 ;
      signalTimeOut = myTime + 40 ;
      signalMode = 1 ;
      signalSwitchLED (true) ;
      break ;
      //
    case 1 :
      //
      if (myTime > signalTimeOut) {
        //
        signalTimeOut = myTime + 120 ;
        signalMode = 2 ;
        signalSwitchLED (false) ;
        //
        if (++ signalCounter > signalRepetitions) {
          //
          signalMode = 10 ;
          //
        }
        //
      }
      break ;
      //
    case 2 :
      //
      if (myTime > signalTimeOut) {
        //
        signalTimeOut = myTime + 40 ;
        signalMode = 1 ;
        signalSwitchLED (true) ;
        //
      }
      break ;
      //
    case 3 :
      //
      break ;
      //
    case 10 :
      //
      signalSwitchLED (state) ;
      //
      break ;
      //
    default:
      //
      break ;
  }
  //
}