//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
//
#include "global.h"
#include "pulser.h"
#include "logger.h"
//
template <rpacPin_t p> unsigned long rpac::Pulser<p>::change {0} ;
template <rpacPin_t p> int rpac::Pulser<p>::stage {0} ;
template <rpacPin_t p> int rpac::Pulser<p>::cycle {0} ;
//
template <rpacPin_t p> bool rpac::Pulser<p>::pulse {false} ;
template <rpacPin_t p> unsigned long rpac::Pulser<p>::dummy {12345678} ;
template <rpacPin_t p> typename rpac::Pulser <p>::Mode rpac::Pulser<p>::mode {rpac::Pulser <p>::Mode::mBase} ;
#if defined(ARDUINO_SEEED_XIAO_RP2040)
template <rpacPin_t p> RP2040_PWM * rpac::Pulser <p>::_PWM_Instance {nullptr} ;
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_freq {7000.0f} ;
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_full {32.5f} ; // 100 % duty cycle gives 2.0 Volts input to PIN3 of OP-Amp IC3A means 0.2 MPa (Voltage - 0.2)/9 
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_zero {0.0f} ;
#endif
//
#if false
constexpr int vars{1} ;
template <rpacPin_t p, rpac::rpacPin_t s> const unsigned long __on [vars]{0} ;
template <rpacPin_t p, rpac::rpacPin_t s> const unsigned long __off [vars]{10000} ;
template <rpacPin_t p, rpac::rpacPin_t s> const int __cycles [vars]{1} ;
#else
constexpr int vars{6} ;
template <rpacPin_t p> const unsigned long rpac::Pulser <p>::__on [vars]{0, 2000, 3000, 4000, 5000, 6000} ;
template <rpacPin_t p> const unsigned long rpac::Pulser <p>::__off [vars]{10000, 6000, 5000, 4000, 3000, 2500} ;
template <rpacPin_t p> const int rpac::Pulser <p>::__cycles [vars]{1, 8, 12, 16, 20, 30} ;
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
  Serial.print (static_cast <int> (mode), DEC) ;
  Serial.print ("\t") ;
  Serial.println (cycle, DEC) ;
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
      Serial.print (String (stage, DEC)) ;
      Serial.print ("/") ;
      Serial.print (String (vars - 1, DEC)) ;
      Serial.print (" ... ON = ") ;
      Serial.print (String (__on [stage], DEC)) ;
      Serial.print ("ms, OFF = ") ;
      Serial.print (String (__off [stage], DEC)) ;
      Serial.print ("ms.") ;
      Serial.print (", CYCLES = ") ;
      Serial.println (String (__cycles [stage], DEC)) ;
#endif
      //
    } else {
      //
      mode = Mode::mBase ;
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
template <rpacPin_t p> bool rpac::Pulser <p>::toggle (mode_t m) {
  //
  if (mode == m) {
    //
    mode = Mode::mBase ;
    //
#ifdef __DEBUG__PULSER__
    Serial.println ("[INFO] rpac::Pulser::toggle () switched to base mode.") ;
#endif
    //
    return true ;
    //
  }
  //
  if (mode == Mode::mBase) {
    //
    mode = m ;
    //
#ifdef __DEBUG__PULSER__
    Serial.print ("[INFO] rpac::Pulser::toggle () switched to mode ") ;
    Serial.print (static_cast <int> (m)) ;
    Serial.println (".") ;
#endif
    //
    return true ;
    //
  }
  //
  return false ;
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
    Serial.print (i + 1, DEC) ;
    Serial.print (", OFF = ") ;
    Serial.print (String (__off [i], DEC)) ;
    Serial.print (", ON = ") ;
    Serial.print (String (__on [i], DEC)) ;
    Serial.print (", CYCLES = ") ;
    Serial.print (String (__cycles [i], DEC)) ;
    Serial.println (".") ;
    //
  }
  //
#ifdef ARDUINO_SEEED_XIAO_RP2040
  Serial.print ("\n[INFO] Pulse duty factor ") ;
  Serial.print (_PWM_full, 1) ;
  Serial.print (" % corresponding to ") ;
  Serial.print ((2.0f * 0.01 * _PWM_full - 0.2f)/9.0f, 2) ;
  Serial.println (" MPa pulse pressure offset to inflow.\n") ;
#endif
  //
#endif
}
//
template <rpacPin_t p> bool rpac::Pulser <p>::loop (bool trigger) {
  //
  switch (mode) {
    //
    case Mode::mAuto :
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
    case Mode::mBase :
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
    case Mode::mTune :  // to be implemented
      //
      break ;
      //
    case Mode::mDose :  // to be implemented
      //
      break ;
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