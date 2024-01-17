#include "pressure.h"
//
unsigned long nextSampleTime = 0, sampleAdjust = 5 ;
//
static unsigned long int pressureDataCB (void) {
  //
  return analogRead (pressurePin) ;
  //
}
//
void pressureSetup (loggerCBs_t & callbacks) {
  //
  pinMode (pressurePin, INPUT) ;
  //
  nextSampleTime = sampleInterval ;
  //
  callbacks.add (& pressureDataCB) ;
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