#include <EEPROM.h>
//
#include "global.h"
#include "pulser.h"
#include "sensor.h"
#include "logger.h"
//
const int buttonPin = 16 ;
const unsigned long loopMaxDura = 12 ;
//
volatile unsigned long buttonPressedTime = 0 ;
sensorData_t sensorData ;
//
//  Handles the falling edge interrupt caused by switch closing contacts
//
void handler () {
  //
  buttonPressedTime = millis () ;
  //
}
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  pinMode (LED_BUILTIN, OUTPUT) ;
  pinMode (buttonPin, INPUT_PULLUP) ;
  //
  digitalWrite (LED_BUILTIN, HIGH) ;
  //
  delay (1500) ;
  //
  digitalWrite (LED_BUILTIN, LOW) ;
  //
  Serial.begin (115200) ;
  //
  attachInterrupt(digitalPinToInterrupt(buttonPin), handler, FALLING) ;
  //
  delay (500) ;
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
  //
  if (buttonPressedTime) {
    //
    noInterrupts () ;
    //
    Serial.print ("Button was pressed @ ") ;
    Serial.print (String (buttonPressedTime)) ;
    Serial.println (" ms from start.") ;
    //
    buttonPressedTime = 0 ;
    //
    interrupts () ;
    //
  }
  //
  sensorLoop (sensorData) ;
  //
  loggerLoop (sensorData, "") ;
  //
  pulserLoop (pulserChangeTime, pulserProgressCount) ;
  //
  if (millis () - loopBegin > loopMaxDura) Serial.println ("[WARNING] Maximum outer loop duration of " + String (loopMaxDura) + " exceeded.") ;
  //
}
//