#include "pressure.h"
#include "logger.h"
//
static uint8_t pressurePin ;
//
static unsigned long int pressureDataCB (void) {
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