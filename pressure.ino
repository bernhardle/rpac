#include "pressure.h"
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