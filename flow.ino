//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include "global.h"
#include "flow.h"
#include "logger.h"
//
template <rpacPin_t p> volatile unsigned long rpac::Flow <p>::total {0} ;
//
#ifdef __DEBUG__FLOW__
template <rpacPin_t p> volatile bool rpac::Flow <p>::_hrc_error {false} ;
#endif
//
template <rpacPin_t p> volatile bool rpac::Flow <p>::_hrc_trigger {false} ;
template <rpacPin_t p> const uint8_t rpac::Flow <p>::_hrc_countsPerUnit [_hrc_countsPerUnitSize]{12U, 11U, 12U, 11U, 12U, 11U, 12U, 11U, 12U, 12U} ;
template <rpacPin_t p> volatile unsigned short rpac::Flow <p>::_hrc_lap {0} ; 
template <rpacPin_t p> bool rpac::Flow <p>::_hrc_high {false} ;
template <rpacPin_t p> unsigned int rpac::Flow <p>::_hrc_pos {0} ;
//
template <rpacPin_t p> const short int rpac::Flow <p>::_smv_mult [_smv_sampleSize] {22u, 21u, 21u, 20u, 19u, 18u, 17u, 16u, 15u, 14u, 12u, 11u, 10u, 8u, 6u, 2u} ;
template <rpacPin_t p> short int rpac::Flow <p>::_smv_hBuf [_smv_hBufSize] {0} ;
template <rpacPin_t p> short int rpac::Flow <p>::_smv_pos {0} ;
template <rpacPin_t p> unsigned short int rpac::Flow <p>::_smv_ret {0} ;
template <rpacPin_t p> unsigned long int rpac::Flow <p>::_smv_posUpd {0} ;
//
template <rpacPin_t p> void rpac::Flow <p>::_handler (void) {
  //
  total ++ ;
  //
  _smv_hBuf [_smv_pos] ++ ;
  //
  if (++ _hrc_lap == _hrc_countsPerUnit [_hrc_pos]) {
    //
#ifdef __DEBUG__FLOW__
    _hrc_error = trigger ;  // 'trigger' should have been cleared at this time - otherwise flag error condition
#endif
    //
    _hrc_trigger = _hrc_high ? true : _hrc_pos == 0 ;
    //
    _hrc_lap = 0 ;
    //
    if (++ _hrc_pos == _hrc_countsPerUnitSize) _hrc_pos = 0 ;
    //
  }
  //
}
//
template <rpacPin_t p> bool rpac::Flow <p>::resox (void) {
  //
  if (_hrc_high) {
    //
    _hrc_high = false ;
    //
#ifdef __DEBUG__FLOW__
    Serial.println ("[INFO] flowControlCB () switched to low resolution 1:1") ;
#endif
    return true ;
    //
  } else {
    //
    _hrc_high = true ;
    //
#ifdef __DEBUG__FLOW__
    Serial.println ("[INFO] flowControlCB () switched to high resolution 1:10") ;
#endif
    return false ;
    //
  }
  //
}
//
template <rpacPin_t p> void rpac::Flow <p>::setup (loggerCBs_t & lcbs) {
  //
#if defined(ARDUINO_SEEED_XIAO_RP2040) || defined(NANO_RP2040_CONNECT)
  pinMode (static_cast <uint8_t> (p), INPUT_PULLUP) ;
#else
  pinMode (static_cast <uint8_t> (p), INPUT) ;  // Pin is allowed to float as there is a 4.7k pullup in the flow counter for the Nano Every
#endif
  //
  attachInterrupt(digitalPinToInterrupt(static_cast <uint8_t> (p)), & _handler, FALLING) ;
  //
  lcbs.add ([]() -> unsigned long { return total ; }, "Flow PIN" + String (static_cast <int> (p), DEC)) ;
  lcbs.add ([]() -> unsigned long { return static_cast <unsigned long int> (_smv_ret) ; }, "SWMV") ;
  //
  _smv_pos = 0 ;
  //
  _smv_posUpd = millis () ;
  //
}
//
template <rpacPin_t p> typename rpac::Flow <p>::flow_t rpac::Flow <p>::loop (void) {
  //
  unsigned long int myTime {millis()} ;
  //
  while (_smv_posUpd < myTime) {
    //
    long int ret {0} ;
    //
    _smv_pos = (_smv_pos + 1) % _smv_hBufSize ;
    //
    _smv_hBuf [_smv_pos] = 0 ;
    //
    _smv_posUpd += _smv_sampleInterval ;
    //
    for (int i = 0, j = _smv_pos + _smv_sampleSize ; i < _smv_sampleSize ; i ++ ) {
      //
      ret += _smv_hBuf [j -- % _smv_sampleSize] * _smv_mult [i] ;
      //
    }
    //
    _smv_ret = static_cast <short int> ((1000u * ret) / _smv_div ()) ;
    //
  }
  //
  if (_hrc_trigger) {
    //
    _hrc_trigger = false ;
    //
#ifdef __DEBUG__FLOW__
    Serial.println ("[INFO] Flow meter pulsed.") ;
#endif
    //
    return true ;
    //
  }
  //
#ifdef __DEBUG__FLOW__
  if (_hrc_error) {
    //
    Serial.println ("[WARNING] Flow trigger error flag set.") ;
    //
    _hrc_error = false ;
  }
#endif
  //
  return false ;
  //
}
//
#ifdef ARDUINO_SEEED_XIAO_RP2040
//
template <rpacPin_t p> void rpac::Flow <p>::loop1 (void) {
  //
  static unsigned long int next {0} ;
  //
  unsigned long int myTime = millis () ;
  //
  if (myTime > next) {
    //
    _handler () ;
    //
    next = myTime + 300 ;
    //
  }
  //
}
#endif