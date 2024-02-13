//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "button.h"
#include "logger.h"
//
template <rpac::rpacPin_t p> volatile unsigned long int rpac::Button <p>::time{0} ;
template <rpac::rpacPin_t p> unsigned long int rpac::Button <p>::last{0} ;
template <rpac::rpacPin_t p> unsigned short int rpac::Button <p>::count{0} ;
        //
#ifdef __DEBUG__BUTTON__
template <rpac::rpacPin_t p> bool rpac::Button <p>::check{false} ;
#endif
//
//  Handles the falling edge interrupt caused by switch closing contacts
//
template <rpac::rpacPin_t p> void rpac::Button <p>::handler (void) {
  //
  time = millis () ;
  //
}
//
template <rpac::rpacPin_t p> void rpac::Button <p>::setup (loggerCBs_t & lcbs) {
  //
  pinMode (static_cast<int> (p), INPUT_PULLUP) ;
  //
  attachInterrupt(digitalPinToInterrupt(static_cast<int>(p)), & handler, FALLING) ;
  //
  String label = String ("Button PIN") + String(static_cast<int>(p), DEC) ;
  //
  lcbs.add ([]() -> unsigned long { return BUTTONSTATE(count) ; }, label) ;
  //
}
//
template <rpac::rpacPin_t p> bool rpac::Button <p>::loop (void) {
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