//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
//
#include "global.h"
#include "pulser.h"
#include "logger.h"
//
using Data = rpac::BTLogger ;
//
template <rpacPin_t p> uint32_t rpac::Pulser<p>::change {0u} ;
template <rpacPin_t p> uint32_t rpac::Pulser<p>::endTime {0u} ;
template <rpacPin_t p> uint16_t rpac::Pulser<p>::stage {0u} ;
template <rpacPin_t p> uint16_t rpac::Pulser<p>::cycle {0u} ;
//
template <rpacPin_t p> bool rpac::Pulser<p>::pulse {false} ;
template <rpacPin_t p> typename rpac::Pulser <p>::Mode rpac::Pulser<p>::mode {rpac::Pulser <p>::Mode::mBase} ;
//
#if defined(__RPAC__ANALOG__PULSE__)
//  
//  100 % duty cycle gives 1.5 Volts input to PIN3 of OP-Amp IC3A means 0.2 MPa (Voltage - 0.2)/9
//
template <rpacPin_t p> uint8_t rpac::Pulser<p>::_PWM_full {80u} ;
template <rpacPin_t p> uint8_t rpac::Pulser<p>::_PWM_zero {0u} ;
#if defined(__RPAC__RP2040__PWM__) || defined(__RPAC__NRF52__PWM__) || defined(__RPAC__NRF52__MBED__PWM__)
template <rpacPin_t p> typename rpac::Pulser <p>::_PWM_instance_t * rpac::Pulser <p>::_PWM_Instance {nullptr} ;
template <rpacPin_t p> float rpac::Pulser<p>::_PWM_freq {7000.0f} ;
#endif
#endif
/*
constexpr int vars{5} ;
template <rpacPin_t p> const uint32_t rpac::Pulser <p>::__on [vars]{0, 2000, 3000, 4000, 5000} ;
template <rpacPin_t p> const uint32_t rpac::Pulser <p>::__off [vars]{10000, 6000, 5000, 4000, 3000} ;
template <rpacPin_t p> const uint16_t rpac::Pulser <p>::__cycles [vars]{1, 10, 15, 20, 25} ;
*/
//
constexpr int vars{1} ;
template <rpacPin_t p> const uint32_t rpac::Pulser <p>::__on [vars] {3000u} ;
template <rpacPin_t p> const uint32_t rpac::Pulser <p>::__off [vars] {2000u} ;
template <rpacPin_t p> const uint16_t rpac::Pulser <p>::__cycles [vars] {32u} ;
//
template <rpacPin_t p> inline void rpac::Pulser <p>::__pulseOn (void) {
  //
#if defined(__RPAC__RP2040__PWM__) || defined(__RPAC__NRF52__PWM__)
  _PWM_Instance->setPWM (static_cast <uint8_t> (p), _PWM_freq, static_cast <float> (_PWM_full)) ;
#elif defined(__RPAC__NRF52__MBED__PWM__)
  setPWM (_PWM_Instance, static_cast <uint8_t> (p), _PWM_freq, static_cast <float> (_PWM_full)) ;
#elif defined(__RPAC__MBED__PWM__)
  analogWrite (static_cast <uint8_t> (p), static_cast <uint32_t> (2.5499f * static_cast <float> (_PWM_full))) ;
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
#if defined(__RPAC__RP2040__PWM__) || defined(__RPAC__NRF52__PWM__)
  _PWM_Instance->setPWM (static_cast <uint8_t> (p), _PWM_freq, static_cast <float> (_PWM_zero)) ;
#elif defined(__RPAC__NRF52__MBED__PWM__)
  setPWM (_PWM_Instance, static_cast <uint8_t> (p), _PWM_freq, static_cast <float> (_PWM_zero)) ;
#elif defined(__RPAC__MBED__PWM__)
  analogWrite (static_cast <uint8_t> (p), (2.5499f * static_cast <float> (_PWM_zero))) ;
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
    Serial.print (static_cast <uint8_t> (m)) ;
    Serial.println (".") ;
#endif
    //
    if (mode == Mode::mBLE) Data::start () ;
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
  __pulseOff () ;
  //
  lcbs.add ([](void) -> unsigned long {
#if defined(__RPAC__ANALOG__PULSE__)
    return static_cast <uint16_t> (pulse ? _PWM_full : _PWM_zero) ;
#else
    return static_cast <uint16_t> (pulse) ;
#endif
  }, String ("Pulse PIN") + String (static_cast <uint8_t> (p), DEC)) ;
  //
  stage = 0 ;
  pulse = false ;
  cycle = 0 ;
  change = millis () ;
  endTime = change ;
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
#if defined(__RPAC__ANALOG__PULSE__)
  Serial.print ("\n[INFO] Pulse duty factor ") ;
  Serial.print (_PWM_full, DEC) ;
  Serial.print (" % corresponding to ") ;
  Serial.print ((1.5f * 0.01 * static_cast <float> (_PWM_full) - 0.2f)/9.0f, 2) ;
  Serial.println (" MPa pulse pressure offset to inflow.\n") ;
#endif
  //
#endif
}
//
template <rpacPin_t p> bool rpac::Pulser <p>::loop (bool trigger) {
  //
  uint32_t myTime = millis () ;
  //
  switch (mode) {
    //
    case Mode::mAuto :
      //
      {
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
    case Mode::mBLE :
      //
      if (pulse && myTime > endTime) {
        //
        __pulseOff () ;
        //
        break ;
      }
      // 
      if (!pulse && myTime < endTime) {
        //
        __pulseOn () ;
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
//
template <rpacPin_t p> bool rpac::Pulser <p>::remotePulse (uint16_t duration) {
  //
#ifdef __DEBUG__PULSER__
  Serial.print ("[INFO] Remote pulse trigger for ") ;
  Serial.print (duration) ;
  Serial.println (" ms.") ;
#endif  
  if (mode != Mode::mBLE) return false ;
  //
  endTime = (duration > maxRemotePulseDurationMS ? maxRemotePulseDurationMS : duration) + millis () ;
  //
  return true ;
  //
}
//
template <rpacPin_t p> uint8_t rpac::Pulser <p>::remoteDuty (uint8_t nValue) {
  //
  uint8_t oValue {_PWM_full} ;
  //
  if (nValue > 100) {
    //
    Serial.print ("[WARNING] Invalid remote request for duty cycle change '") ;
    Serial.print (nValue) ;
    Serial.println ("'. Ignored") ;
    //
  } else {
    //
#if defined(__INFO__PULSER__) || defined(__DEBUG__PULSER__)
    Serial.print ("[INFO] Remote request for duty cycle change ") ;
    Serial.print (oValue) ;
    Serial.print (" % -> ") ;
    Serial.print (nValue) ;
    Serial.println (" %.") ;
#endif
    //
    _PWM_full = nValue ;
    //
  }
  return oValue ;
  //
}