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
static RTC_PCF8523 rtc ;
//
static String timeDateTime2Stamp (const DateTime & dat) {
  //
  String stamp (dat.year(), DEC) ;
  stamp.concat ('-') ;
  stamp.concat (dat.month () > 9 ? "" :"0") ;
  stamp.concat (String (dat.month(), DEC)) ;
  stamp.concat ('-') ;
  stamp.concat (dat.day () > 9 ? "" :"0") ;
  stamp.concat (String (dat.day(), DEC)) ;
  stamp.concat (" ") ;
  stamp.concat (dat.hour () > 9 ? "" :"0") ;
  stamp.concat (String (dat.hour(), DEC)) ;
  stamp.concat (':');
  stamp.concat (dat.minute () > 9 ? "" :"0") ;
  stamp.concat (String (dat.minute(), DEC)) ;
  stamp.concat (':') ;
  stamp.concat (dat.second () > 9 ? "" :"0") ;
  stamp.concat (String (dat.second(), DEC)) ;
  //
  return stamp ;
}
//
static unsigned long int timeDataCB (void) {
  //
  return millis() ;
  //
}
//
//
String timeSetup (loggerCBs_t & callbacks) {
  //
  callbacks.add (& timeDataCB, "time") ;
  //
#ifndef ARDUINO_UBLOX_NINA_W10
  Wire.begin() ;
  //
  {
    bool rtcReady = rtc.begin (&Wire) ;
    //
    if (rtcReady) {
      //
      DateTime now = rtc.now () ;
      //
#ifdef __DEBUG__RPAC__
      Serial.print ("[INFO] RTC @ setup is ") ;
      Serial.println (timeDateTime2Stamp (now)) ;
#endif
      //
      return timeDateTime2Stamp (now) ;
      //
    } else {
      //
#ifdef __DEBUG__RPAC__
      Serial.println ("[ERROR] RTC not ready for unknown reasons.") ;
#endif
      //
      return String ("[ERROR] RTC not ready.") ;
      //
    }
  //
  }
#else
  return String ("[INFO] RTC not implemented in NINA W10.") ;
#endif
  //
}