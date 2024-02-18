//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "pressure.h"
#include "logger.h"
//
//
//  Voltage divider is 100 k / 147 k
//  in order to scale max. 5V out < 3.3V
//  pressure zero offset is ~ 33 digits raw
//  MXP555 sensitvity is 9.0 mV / kPa
//  ADC conversion rate is 5 / 1.024 mV / digit 
//
//  Scaling term: float (max (dgs - 33, 0)) * float (1.47 * 5.0 / 1024 / 0.9) ;
//
//
template <rpac::rpacPin_t p> void rpac::Pressure <p>::setup (loggerCBs_t & lcbs) {
  //
  pinMode (static_cast <uint8_t> (p), INPUT) ;
  //
  lcbs.add ([](void) -> unsigned long { return analogRead (static_cast <uint8_t> (p)) ; }, "Pressure PIN" + String (static_cast <int> (p))) ;
  //
}
//
template <rpac::rpacPin_t p> bool rpac::Pressure <p>::loop (void) {
  //
  //  Dummy routine for the moment. Maybe, we wish to add a
  //  sliding window mean value or the like at a later time.
  //
  return false ;
  //
}