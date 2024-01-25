#include "pressure.h"
#include "logger.h"
//
static pin_size_t pressurePin ;
//
static unsigned long int pressureDataCB (void) {
  //
  return analogRead (pressurePin) ;
  //
}
//
void pressureSetup (pin_size_t pin, loggerCBs_t & callbacks) {
  //
  pinMode ((pressurePin = pin), INPUT) ;
  //
  callbacks.add (& pressureDataCB, "pressure") ;
  //
  return ;
}
//
void pressureLoop (void) {
  //
  return ;
  //
}