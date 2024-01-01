#include "global.h"
#include "pulser.h"
#include "sensor.h"
//
unsigned long nextSampleTime = 0, sampleAdjust = 5 ;
//
void sensorSetup (sensorData_t & data) {
  //
  pinMode (sensorPin, INPUT) ;
  //
  data.sample = 0 ;
  data.time = millis () ;
  data.pressure = 0 ;
  data.pulseOn = digitalRead (pulserPin) ;
  //
  nextSampleTime = sampleInterval ;
  //
  return ;
}
//
void sensorLoop (sensorData_t & data) {
  //
  unsigned long myTime = millis (), pause = 0 ;
  //
  if (myTime < nextSampleTime - sampleAdjust) { return ; }
  //
  while (myTime > nextSampleTime) nextSampleTime += sampleInterval ;
  //
  delay (nextSampleTime - myTime) ;
  //
  data.sample ++ ;
  data.time = millis () ;
  data.pressure = analogRead(sensorPin) ;
  data.pulseOn = digitalRead (pulserPin) ;
  //
  nextSampleTime += sampleInterval ;
  //
  return ;
  //
}