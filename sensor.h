#ifndef __sensor_h_included__
#define __sensor_h_included__
//
#include <String.h>
//
const unsigned long sampleInterval = 250 ;
const int sensorPin = A0 ;
//
typedef class sensorData {
  //
  public :
    unsigned long time ;
    unsigned short sample, pressure ;
    bool pulseOn ;
  public :
    sensorData () : time (0), sample (0), pressure (0), pulseOn (false) {}
  //
} sensorData_t ;
//
extern void sensorSetup (void) ;
extern void sensorLoop (sensorData_t &) ;
//
#endif