//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "pulser.h"
#include "logger.h"
//
template <rpacPin_t p> unsigned long rpac::Pulser<p>::change{0} ;
template <rpacPin_t p> unsigned short rpac::Pulser<p>::mode{0} ;
template <rpacPin_t p> unsigned short rpac::Pulser<p>::cycle{0} ;
template <rpacPin_t p> bool rpac::Pulser<p>::pulse{false} ;
template <rpacPin_t p> bool rpac::Pulser<p>::automate{true} ;

//
#if 0
constexpr int vars{1} ;
template <rpacPin_t p, rpac::rpacPin_t s> const unsigned long __on [vars]{0} ;
template <rpacPin_t p, rpac::rpacPin_t s> const unsigned long __off [vars]{10000} ;
template <rpacPin_t p, rpac::rpacPin_t s> const unsigned int __cycles [vars]{1} ;
#else
constexpr int vars{6} ;
template <rpacPin_t p> const unsigned long rpac::Pulser <p>::__on [vars]{0, 2000, 3000, 4000, 5000, 6000} ;
template <rpacPin_t p> const unsigned long rpac::Pulser <p>::__off [vars]{10000, 6000, 5000, 4000, 3000, 2500} ;
template <rpacPin_t p> const unsigned int rpac::Pulser <p>::__cycles [vars]{1, 8, 12, 16, 20, 30} ;
#endif
//
template <rpacPin_t p> inline void rpac::Pulser <p>::__nextCycle (void) {
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
      Serial.print (" ... ON = ") ;
      Serial.print (String(__on [mode], DEC)) ;
      Serial.print ("ms, OFF = ") ;
      Serial.print (String(__off [mode], DEC)) ;
      Serial.print ("ms.") ;
      Serial.print (", CYCLES = ") ;
      Serial.println (String (__cycles [mode], DEC)) ;
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
template <rpacPin_t p> bool rpac::Pulser <p>::autox (void) {
  //
  if (automate) {
    //
    automate = false ;
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] rpac::Pulser::autox () disabled autopulse") ;
#endif
    //
    return false ;
    //
  } else {
    //
    automate = true ;
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] rpac::Pulser::autox () enabled autopulse") ;
#endif
    //
    return true ;
    //
  }
  //
}
//
template <rpacPin_t p> bool rpac::Pulser <p>::autox (bool b) {
  //
  return (automate = b) ;
  //
}
//
template <rpacPin_t p> void rpac::Pulser <p>::setup (loggerCBs_t & lcbs) {
  //
  pinMode (static_cast <uint8_t> (p), OUTPUT) ;
  digitalWrite (static_cast <uint8_t> (p), LOW) ;
  //
  String label = String ("Pulse PIN") + String (static_cast<int> (p), DEC) ;
  //
  lcbs.add ([](void) -> unsigned long { return static_cast <unsigned long> (pulse) ; }, label) ;
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
    Serial.print (", OFF = ") ;
    Serial.print (String (__off [i], DEC)) ;
    Serial.print (String (i, DEC)) ;
    Serial.print (", ON = ") ;
    Serial.print (String (__on [i], DEC)) ;
    Serial.print (", CYCLES = ") ;
    Serial.print (String (__cycles [i], DEC)) ;
    Serial.println (".") ;
    //
  }
#endif
}
//
template <rpacPin_t p> bool rpac::Pulser <p>::loop (bool trigger) {
  //
  if (automate) {
    //
    unsigned long myTime = millis () ;
    //
    if (pulse) {
      //
      if (myTime > change + __on [mode]) {
        //
        change = myTime ;
        //
        digitalWrite (static_cast <uint8_t> (p), LOW) ;
        //
        pulse = false ;
        //
        __nextCycle () ;
        //
      }
      //
    } else {
      //
      if (myTime > change + __off [mode]) {
        //
        change = myTime ;
        //
        //  To manage with 0% modes: __nextCycle () is called at 
        //  the end of pulse in regular cycles where ratio > 0%
        //  but needs to be called here in cycles where ratio = 0%
        //
        if (0 == __on [mode]) {
          //
          __nextCycle () ;
          //
        } else {
          //
          digitalWrite (static_cast <uint8_t> (p), HIGH) ;
          //
          pulse = true ;
          //
        }
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