#include <Wire.h>
//
#include <RTClib.h>
//
#include "rtc.h"
//
#ifdef __DEBUG__RPAC__
const char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"} ;
#endif
//
static RTC_PCF8523 rtc ;
//
static String rtcDateTime2Stamp (const DateTime & dat) {
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
String rtcSetup (void) {
  //
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
      Serial.println (rtcDateTime2Stamp (now)) ;
#endif
      //
      return rtcDateTime2Stamp (now) ;
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
  //
}