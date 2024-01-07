#include "button.h"
//
#define BUTTONSTATE(a)  (a > 7 ? true : false)
//
static volatile unsigned long buttonPressedTime = 0 ;
static signed short buttonPressedCount = 0 ;
//
#ifdef __DEBUG__RPAC__
static bool buttonLastState = false ;
#endif
//
//  Handles the falling edge interrupt caused by switch closing contacts
//
static void handler () {
  //
  buttonPressedTime = millis () ;
  //
}
//
void buttonSetup (void) {
  //
  pinMode (buttonPin, INPUT_PULLUP) ;
  //
  attachInterrupt(digitalPinToInterrupt(buttonPin), handler, FALLING) ;
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
#ifdef __DEBUG__RPAC__
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