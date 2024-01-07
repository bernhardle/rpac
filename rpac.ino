#include <EEPROM.h>
#include <Wire.h>
//
#include <RTClib.h>
//
#include "global.h"
#include "button.h"
#include "pulser.h"
#include "sensor.h"
#include "logger.h"
#include "signal.h"
//
#ifdef __DEBUG__RPAC__
const unsigned long loopMaxDura = 12 ;
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"} ;
#endif
//
sensorData_t sensorData ;
RTC_PCF8523 rtc ;
//
// the setup function runs once when you press reset or power the board
//
void setup() {
  //
  Serial.begin (115200) ;
  //
  Wire.begin() ;
  //
  {
    bool rtcReady = rtc.begin (&Wire) ;
    //
#ifdef __DEBUG__RPAC__
    if (rtcReady) {
      //
      DateTime now = rtc.now () ;
      Serial.print ("RTC @ setup is ") ;
      Serial.print (now.year(), DEC);
      Serial.print ('/');
      Serial.print (now.month(), DEC);
      Serial.print ('/');
      Serial.print (now.day(), DEC);
      Serial.print (" (");
      Serial.print (daysOfTheWeek[now.dayOfTheWeek()]);
      Serial.print (") ");
      Serial.print (now.hour(), DEC);
      Serial.print (':');
      Serial.print (now.minute(), DEC);
      Serial.print (':');
      Serial.println (now.second(), DEC) ;
      //
    } else {
      //
      Serial.println ("RTC not ready for unknown reasons.") ;
      //
    }
#endif
    signalSetup () ;
    //
    buttonSetup () ;
    //
    loggerSetup (rtcReady ? rtc.now () : DateTime ()) ;
    //
    sensorSetup (sensorData) ;
    //
  }
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