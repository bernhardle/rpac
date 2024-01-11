#include "relais.h"
//
const int relaisPin = 2 ;
//
bool relaisOn = false ;
//
void relaisSetup (void) {
  //
  pinMode (relaisPin, OUTPUT) ;
  digitalWrite (relaisPin, LOW) ;
  //
}
//
void relaisLoop (bool state) {
  //
  if (relaisOn != state) {
    digitalWrite (relaisPin, (relaisOn = state)) ;
  }
  //
}
