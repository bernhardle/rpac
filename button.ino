#include "button.h"
//
#define BUTTONSTATE(a)  (a > 7 ? true : false)
//
static volatile unsigned long buttonPressedTime = 0 ;
static signed short buttonPressedCount = 0 ;
//
#ifdef __DEBUG__BUTTON__
static bool buttonLastState = false ;
#endif
//
//  Handles the falling edge interrupt caused by switch closing contacts
//
static void buttonIntHandler () {
  //
  buttonPressedTime = millis () ;
  //
}
//
static unsigned long int buttonDataCB (void) {
  //
  return BUTTONSTATE(buttonPressedCount) ;
  //
}
//
void buttonSetup (loggerCBs_t & callbacks) {
  //
  pinMode (buttonPin, INPUT_PULLUP) ;
  //
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonIntHandler, FALLING) ;
  //
  callbacks.add (& buttonDataCB, "button") ;
  //
}
//
bool buttonLoop (void) {
  //
  unsigned long myTime = millis () ;
  //
  if (myTime > buttonPressedTime + 20) {
    //
    buttonPressedCount = digitalRead (buttonPin) ? max (buttonPressedCount - 1, 0) : min (buttonPressedCount + 1, 15) ;
    //
  } 
  //
#ifdef __DEBUG__BUTTON__
  //
  if (BUTTONSTATE(buttonPressedCount) != buttonLastState) {
    //
    buttonLastState = BUTTONSTATE(buttonPressedCount) ;
    //
    Serial.println (String (BUTTONSTATE(buttonPressedCount) ? "[INFO] Button was pressed @" : "[INFO] Button was released @ ") + String (millis(), DEC)) ;
    //
  }
#endif
  //
  return BUTTONSTATE(buttonPressedCount) ;
  //
}