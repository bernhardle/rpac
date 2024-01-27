#include "button.h"
#include "logger.h"
//
#define BUTTONSTATE(a)  (a > 7 ? true : false)
//
volatile unsigned long buttonPressedTime = 0 ;
static signed short buttonPressedCount = 0 ;
static pin_size_t buttonPin ;
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
void buttonSetup (pin_size_t pin, loggerCBs_t & callbacks) {
  //
  pinMode ((buttonPin = pin), INPUT_PULLUP) ;
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
//