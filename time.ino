//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include <Wire.h>
#include <RTClib.h>
//
#include "global.h"
#include "time.h"
#include "logger.h"
//
template <class T> T rpac::Time <T>::rtc ;
template <class T> DateTime rpac::Time <T>::now ;
template <class T> unsigned long rpac::Time <T>::bias{0} ;
template <class T> char rpac::Time <T>::stamp [64]{"Time"} ;
//
template <class T> void rpac::Time <T>::setup (loggerCBs_t & lcbs) {
  //
#ifndef ARDUINO_UBLOX_NINA_W10
  Wire.begin() ;
  //
  {
    bool rtcReady = rtc.begin (&Wire) ;
    //
    if (rtcReady) {
      //
      now = rtc.now () ;
      bias = millis () ;
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("[INFO] RTC successfully read.") ;
#endif
      //
    } else {
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("[ERROR] RTC not ready for unknown reasons.") ;
#endif
      //
    }
  //
  }
#endif
  //
  bias = millis () ;
  //
  sprintf (stamp, "%04hu-%02hu-%02hu %02hu:%02hu:%02hu [%08lu]", static_cast <unsigned short> (now.year()), static_cast <unsigned short> (now.month()), static_cast <unsigned short> (now.day()), static_cast <unsigned short> (now.hour()), static_cast <unsigned short> (now.minute ()), static_cast <unsigned short> (now.second ()), bias) ;
  //
  lcbs.add ([]() -> unsigned long { return millis () - bias ; }, String (stamp)) ;
  //
}
//