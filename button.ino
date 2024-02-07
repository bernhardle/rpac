//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "button.h"
#include "logger.h"
//
template <rpacPin_t p> Array <unsigned long int, 32> rpac::DebouncedButton <p>::times{0} ;
template <rpacPin_t p> Array <unsigned short int, 32> rpac::DebouncedButton <p>::counts{0} ;
//
//  Handles the falling edge interrupt caused by switch closing contacts
//
template <rpacPin_t p> void rpac::DebouncedButton <p>::intHandler (void) {
  //
  times.at(static_cast <int> (p)) = millis () ;
  //
}
//
//  Writes to the data log
//
template <rpacPin_t p> unsigned long int rpac::DebouncedButton <p>::dataCB (void) {
  //
  return BUTTONSTATE(counts.at(static_cast<int> (p))) ;
  //
}
//
template <rpacPin_t p> void rpac::DebouncedButton <p>::setup (loggerCBs_t & callbacks) {
  //
  pinMode (static_cast<int> (p), INPUT_PULLUP) ;
  //
  attachInterrupt(digitalPinToInterrupt(static_cast<int>(p)), & intHandler, FALLING) ;
  //
  callbacks.add (& dataCB, "button") ;
  //
}
//
template <rpacPin_t p> bool rpac::DebouncedButton <p>::loop (void) {
  //
  unsigned long myTime = millis () ;
  //
  if (myTime > time + 20) {
    //
    count = digitalRead (static_cast<int>(p)) ? max (count - 1, 0) : min (count + 1, 15) ;
    //
  } 
  //
#ifdef __DEBUG__BUTTON__
  //
  if (BUTTONSTATE(count) != check) {
    //
    check = BUTTONSTATE(count) ;
    //
    Serial.println (String (BUTTONSTATE(count) ? "[INFO] Button was pressed @" : "[INFO] Button was released @ ") + String (millis(), DEC)) ;
    //
  }
#endif
  //
  return BUTTONSTATE(count) ;
  //
}
//
template <rpacPin_t p> bool rpac::DebouncedButton <p>::pressed (void) {
  //
  return time > last ? (last = time, true) : false ;
  //
}