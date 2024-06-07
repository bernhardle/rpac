//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
//
#include "global.h"
#include "pulser.h"
#include "logger.h"
//
template <rpacPin_t p> unsigned long rpac::Pulser<p>::change {0} ;
template <rpacPin_t p> unsigned short rpac::Pulser<p>::stage {0} ;
template <rpacPin_t p> unsigned short rpac::Pulser<p>::cycle {0} ;
//
template <rpacPin_t p> bool rpac::Pulser<p>::pulse {false} ;
template <rpacPin_t p> typename rpac::Pulser <p>::Mode rpac::Pulser<p>::mode {rpac::Pulser <p>::Mode::hand} ;
#if defined(ARDUINO_SEEED_XIAO_RP2040)
template <rpacPin_t p> RP2040_PWM * rpac::Pulser <p>::_PWM_Instance {nullptr} ;
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_freq {7000.0f} ;
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_full {100.0f} ;
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_zero {0.0f} ;
#endif
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
template <rpacPin_t p> inline void rpac::Pulser <p>::__pulseOn (void) {
  //
#ifdef ARDUINO_SEEED_XIAO_RP2040
  _PWM_Instance->setPWM (static_cast <uint8_t> (p), _PWM_freq, _PWM_full) ;
#else
  digitalWrite (static_cast <uint8_t> (p), HIGH) ;
#endif
  //
  pulse = true ;
  //
}
//
template <rpacPin_t p> inline void rpac::Pulser <p>::__pulseOff (void) {
  //
#ifdef ARDUINO_SEEED_XIAO_RP2040
  _PWM_Instance->setPWM (static_cast <uint8_t> (p), _PWM_freq, _PWM_zero) ;
#else
  digitalWrite (static_cast <uint8_t> (p), LOW) ;
#endif
  //
  pulse = false ;
  //
}
//
template <rpacPin_t p> inline void rpac::Pulser <p>::__nextCycle (void) {
  //
  if (++ cycle > __cycles [stage]) {
    //
    if (stage < vars - 1) {
      //
      stage ++ ;
      cycle = 0 ;
      //
#ifdef __DEBUG__PULSER__
      Serial.print ("[INFO] Switching auto pulse to mode ") ;
      Serial.print (String(stage, DEC)) ;
      Serial.print ("/") ;
      Serial.print (String(vars - 1, DEC)) ;
      Serial.print (" ... ON = ") ;
      Serial.print (String(__on [stage], DEC)) ;
      Serial.print ("ms, OFF = ") ;
      Serial.print (String(__off [stage], DEC)) ;
      Serial.print ("ms.") ;
      Serial.print (", CYCLES = ") ;
      Serial.println (String (__cycles [stage], DEC)) ;
#endif
      //
    } else {
      //
      mode = Mode::hand ;
      stage = 0 ;
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
  switch (mode) {
    //
    case Mode::run :
      //
      mode = Mode::hand ;
#ifdef __DEBUG__PULSER__
      Serial.println ("[INFO] rpac::Pulser::autox () disabled autopulse") ;
#endif
      //
      return false ;
      //
    case Mode::hand :
      //
      mode = Mode::run ;
#ifdef __DEBUG__PULSER__
      Serial.println ("[INFO] rpac::Pulser::autox () enabled autopulse") ;
#endif
      //
      return true ;
      //
    default :
      //
      return false ;
      //
  }
  //
}
//
template <rpacPin_t p> bool rpac::Pulser <p>::autox (bool b) {
  //
  switch (mode) {
    //
    case Mode::run :
      //
      if (!b) mode = Mode::hand ;
      //
      break ;
      //
    case Mode::hand :
      //
      if (b) mode = Mode::run ;
      //
      break ;
      //
    default :
      //
      break ;
  }
  //
  return b ;
  //
}
//
template <rpacPin_t p> void rpac::Pulser <p>::setup (loggerCBs_t & lcbs) {
  //
#ifdef ARDUINO_SEEED_XIAO_RP2040
  _PWM_Instance = new RP2040_PWM (static_cast <uint8_t> (p), _PWM_freq, _PWM_zero) ;
#else
  pinMode (static_cast <uint8_t> (p), OUTPUT) ;
  digitalWrite (static_cast <uint8_t> (p), LOW) ;
#endif
  //
  String label = String ("Pulse PIN") + String (static_cast<int> (p), DEC) ;
  //
  lcbs.add ([](void) -> unsigned long { return static_cast <unsigned long> (pulse) ; }, label) ;
  //
  stage = 0 ;
  pulse = false ;
  cycle = 0 ;
  change = millis () ;
  //
#ifdef __DEBUG__PULSER__
  Serial.println ("[INFO] Pulse pattern variants:") ;
  //
  for (int i = 0 ; i < vars ; i++) {
    //
    Serial.print ("\tSTAGE ") ;
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
  switch (mode) {
    //
    case Mode::run :
      //
      {
        //
        unsigned long myTime = millis () ;
        //
        if (pulse) {
          //
          if (myTime > change + __on [stage]) {
            //
            change = myTime ;
            //
            __pulseOff () ;
            //
            __nextCycle () ;
            //
          }
          //
        } else {
          //
          if (myTime > change + __off [stage]) {
            //
            change = myTime ;
            //
            //  To manage with 0% modes: __nextCycle () is called at 
            //  the end of pulse in regular cycles where ratio > 0%
            //  but needs to be called here in cycles where ratio = 0%
            //
            if (0 == __on [stage]) {
              //
              __nextCycle () ;
              //
            } else {
              //
              __pulseOn () ;
              //
            }
            //
          }
          //
        }
        //
        break ;
        //
      }
      //
    case Mode::hand :
      //
      {
        //
        if (trigger != pulse) {
          //
          (pulse = trigger) ? __pulseOn () : __pulseOff () ;
          //
    #ifdef __DEBUG__PULSER__
          Serial.println (trigger ? "[INFO] Pulse manually started." : "[INFO] Pulse manually stopped.") ;
    #endif
          change = millis () ;
          //
        }
        //
        break ;
        //
      }
      //
    default :
      //
      break ;
      //
  }
  //
  return pulse ;
  //
}