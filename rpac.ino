#include <EEPROM.h>
//
#include "global.h"
#include "button.h"
#include "pulser.h"
#include "sensor.h"
#include "logger.h"
#include "signal.h"
//
const unsigned long loopMaxDura = 12 ;
//
sensorData_t sensorData ;
//
bool lastButtonState = false ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  signalSetup () ;
  //
  buttonSetup () ;
  //
  sensorSetup (sensorData) ;
  //
  loggerSetup () ;
  //
  pulserSetup (pulserChangeTime) ;
  //
}
//
// the loop function runs over and over again forever
//
void loop() {
  //
  unsigned long loopBegin = millis () ;
  bool pulseState, buttonState ;
  //
  buttonState = buttonLoop () ;
  //
  if (buttonState != lastButtonState) {
    //
    lastButtonState = buttonState ;
    //
  #ifdef __DEBUG__RPAC__
      Serial.println (String ("[INFO] Button was ") + String (buttonState ? "pressed @" : "released @") + String (millis(), DEC) + " ms.") ;
  #endif
    //
  }
  //
  sensorLoop (sensorData) ;
  //
  loggerLoop (sensorData, "") ;
  //
  pulseState = pulserLoop (pulserChangeTime, pulserProgressCount, buttonState) ;
  //
  signalLoop (pulseState) ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
}
//