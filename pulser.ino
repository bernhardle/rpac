//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "pulser.h"
#include "logger.h"
#include "signal.h"
#include "control.h"
//
template <rpac::rpacPin_t p> unsigned long int rpac::Pulser<p>::change{0} ;
template <rpac::rpacPin_t p> unsigned short int rpac::Pulser<p>::mode{0} ;
template <rpac::rpacPin_t p> unsigned short int rpac::Pulser<p>::cycle{0} ;
template <rpac::rpacPin_t p> bool rpac::Pulser<p>::pulse{false} ;
template <rpac::rpacPin_t p> bool rpac::Pulser<p>::automate{true} ;

//
#if 0
constexpr int vars{1} ;
template <rpac::rpacPin_t p> const unsigned long __on [vars] = {0} ;
template <rpac::rpacPin_t p> const unsigned long __off [vars] = {10000} ;
template <rpac::rpacPin_t p> const unsigned int __cycles [vars] = {1} ;
#else
constexpr int vars{6} ;
template <rpac::rpacPin_t p> const unsigned long rpac::Pulser <p>::__on [vars]{0, 2000, 3000, 4000, 5000, 6000} ;
template <rpac::rpacPin_t p> const unsigned long rpac::Pulser <p>::__off [vars]{10000, 6000, 5000, 4000, 3000, 2500} ;
template <rpac::rpacPin_t p> const unsigned int rpac::Pulser <p>::__cycles [vars]{1, 6, 8, 14, 20, 25} ;
#endif
//
template <rpac::rpacPin_t p> void rpac::Pulser <p>::setup (controlCBs_t & ccbs, loggerCBs_t & lcbs) {
  //
  pinMode (static_cast <uint8_t> (p), OUTPUT) ;
  digitalWrite (static_cast <uint8_t> (p), LOW) ;
  //
  String label = String ("Pulse PIN") + String (static_cast<int> (p), DEC) ;
  //
  lcbs.add ([](void) -> unsigned long int { return pulse ; }, label) ;
  ccbs.add ([](uint8_t) -> uint8_t {
    //
    if (automate) {
      //
      automate = false ;
#ifdef __DEBUG__PULSER__
      Serial.println ("[INFO] pulserControlCB () disabled autopulse") ;
#endif
      //
      // rpac::Signal <s>::async (sig::scheme::blinkfast, 2) ;
      //
      return 0U ;
      //
    } else {
      //
      automate = true ;
#ifdef __DEBUG__PULSER__
      Serial.println ("[INFO] pulserControlCB () enabled autopulse") ;
#endif
      //
      return 1U ;
      //
    }
    //
    return 0 ;
    //
  }, 2) ;
  //
  mode = 0 ;
  pulse = false ;
  cycle = 0 ;
  change = millis () ;
  //
#ifdef __DEBUG__PULSER__
  Serial.println ("[INFO] Pulse pattern variants:") ;
  //
  for (int i = 0 ; i < vars ; i++) {
    //
    Serial.print ("\tMode ") ;
    Serial.print (String (i, DEC)) ;
    Serial.print (", ON = ") ;
    Serial.print (String (__on [i], DEC)) ;
    Serial.print (", OFF = ") ;
    Serial.print (String (__off [i], DEC)) ;
    Serial.print (", CYCLES = ") ;
    Serial.print (String (__cycles [i], DEC)) ;
    Serial.println (".") ;
    //
  }
#endif
}
//
template <rpac::rpacPin_t p> bool rpac::Pulser <p>::loop (bool trigger) {
  //
  if (automate) {
    //
    unsigned long myTime = millis () ;
    //
    if (pulse) {
      //
      if (myTime > change + __on [mode]) {
        //
        digitalWrite (static_cast <uint8_t> (p), LOW) ;
        //
        change = myTime ;
        pulse = false ;
        //
        if (++ cycle > __cycles [mode]) {
          //
          if (mode < vars - 1) {
            //
            mode ++ ;
            cycle = 0 ;
            //
#ifdef __DEBUG__PULSER__
            Serial.print ("[INFO] Switching auto pulse to mode ") ;
            Serial.print (String(mode, DEC)) ;
            Serial.print ("/") ;
            Serial.print (String(vars - 1, DEC)) ;
            Serial.print (" ... ON=") ;
            Serial.print (String(__on [mode], DEC)) ;
            Serial.print ("ms, OFF=") ;
            Serial.print (String(__off [mode], DEC)) ;
            Serial.println ("ms.") ;
#endif
            //
          } else {
            //
            automate = false ;
            mode = 0 ;
            //
#ifdef __DEBUG__PULSER__
            Serial.println ("[INFO] Disabeling auto pulse.") ;
#endif
            //
          }
          //
        }
        //
      }
      //
    } else {
      //
      if (myTime > change + __off [mode]) {
        //
        digitalWrite (static_cast <uint8_t> (p), HIGH) ;
        //
        change = myTime ;
        pulse = true ;
        //
      }
      //
    }
    //
  } else {
    //
    if (trigger != pulse) {
      //
      digitalWrite (static_cast <uint8_t> (p), (pulse = trigger) ? HIGH : LOW) ;
      //
#ifdef __DEBUG__PULSER__
      Serial.println (trigger ? "[INFO] Pulse manually started." : "[INFO] Pulse manually stopped.") ;
#endif
      change = millis () ;
      //
    }
    //
  }
  //
  return pulse ;
  //
}