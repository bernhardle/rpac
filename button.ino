#include "button.h"
//
volatile unsigned long buttonPressedTime = 0 ;
signed short buttonPressedSample = 0, buttonPressedTotal = 0 ;
bool buttonPressed = false ;
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
    buttonPressedSample = digitalRead (buttonPin) ? max (buttonPressedSample - 1, 0) : min (buttonPressedSample + 1, 15) ;
    //
  } 
  //
  return buttonPressedSample > 7 ? true : false ;
  //
}