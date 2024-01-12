#include "pressure.h"
//
unsigned long nextSampleTime = 0, sampleAdjust = 5 ;
//
void pressureSetup (pressureData_t & data) {
  //
  pinMode (pressurePin, INPUT) ;
  //
  data.sample = 0 ;
  data.time = millis () ;
  data.pressure = 0 ;
  //
  nextSampleTime = sampleInterval ;
  //
  return ;
}
//
void pressureLoop (pressureData_t & data) {
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
  data.pressure = analogRead(pressurePin) ;
  //
  nextSampleTime += sampleInterval ;
  //
  return ;
  //
}