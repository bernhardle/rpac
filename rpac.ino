#include <EEPROM.h>
//
#include "global.h"
#include "button.h"
#include "pulser.h"
#include "sensor.h"
#include "logger.h"
#include "signal.h"
#include "rtc.h"
//
#ifdef __DEBUG__RPAC__
const unsigned long loopMaxDura = 12 ;
#endif
//
sensorData_t sensorData ;
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
  loggerSetup (setupRTC ()) ;
  //
  sensorSetup (sensorData) ;
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
  pulseState = pulserLoop (pulserChangeTime, pulserProgressCount, buttonState) ;
  //
  signalLoop (pulseState) ;
  //
  sensorLoop (sensorData) ;
  //
  loggerLoop (sensorData, pulseState, "[]") ;
  //
#ifdef __DEBUG__RPAC__
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Outer loop exceeded " + String (loopMaxDura) + " ms.") ;
#endif
  //
}
//