#include "pressure.h"
#include "logger.h"
//
static uint8_t pressurePin ;
//
static unsigned long int pressureDataCB (void) {
  //
  //  Voltage divider is 100 k / 147 k
  //  in order to scale max. 5V out < 3.3V
  //  pressure zero offset is ~ 33 digits raw
  //  MXP555 sensitvity is 9.0 mV / kPa
  //  ADC conversion rate is 5 / 1.024 mV / digit 
  //
  //  Scaling term: float (max (dgs - 33, 0)) * float (1.47 * 5.0 / 1024 / 0.9) ;
  //
  return analogRead (pressurePin) ;
  //
}
//
void pressureSetup (rpacPin_t pin, loggerCBs_t & lcbs) {
  //
  pinMode ((pressurePin = static_cast <uint8_t> (pin)), INPUT) ;
  //
  lcbs.add (& pressureDataCB, "pressure") ;
  //
  return ;
}
//
void pressureLoop (void) {
  //
  return ;
  //
}