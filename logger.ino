//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include <RTClib.h>
//
#include "global.h"
#include "button.h"
#include "logger.h"
#include "signal.h"
//
unsigned long rpac::Logger::loggerNextSampleTime{0}, rpac::Logger::loggerShutdownFlushTime{0} ;
uint8_t rpac::Logger::mode{5U} ;
uint8_t rpac::Logger::pin ;
bool rpac::Logger::enable{true} ;
//
bool loggerCBs::add (unsigned long (*f)(void), const String & h) {
  //
  if (num < mcbs) {
    //
    cb [num] = f ;
    //
    strncpy (hd [num], h.substring(0,mhdl).c_str (), mhdl) ;
    //
    hd [num][mhdl] = static_cast <char> (0x0) ;
    //
    num ++ ;
    //
    Serial.println (num) ;
    //
    return true ;
  } 
  //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[WARNING] Number of callbacks reached.") ;
#endif
  return false ;
//
}
//
const char * loggerCBs::logRow (const String & message) {
  //
  char * pos = row ;
  //
  for (int i = 0 ; i < static_cast<int>(num) ; i++) {
    //
    pos += sprintf (pos, "%2lu;", (*cb [i])()) ;
    //
  }
  //
  sprintf (pos, "%s", message.substring (0,16).c_str()) ;
  //
  return row ;
}
//
const char * loggerCBs::headRow (const String & stamp) {
  //
  char * pos = row ;
  //
  for (int i = 0 ; i < static_cast<int> (num) ; i++) {
    //
    pos += sprintf (pos, "%2s;", hd [i]) ;
    //
  }
  //
  pos += sprintf (pos, "%s", stamp.substring (0,48).c_str()) ;
  //
  * pos = 0x0 ;
  //
  return row ;
}
//
template <rpac::rpacPin_t b, rpac::rpacPin_t s> void rpac::Logger::setup (rpac::rpacPin_t p, rpac::controlCBs_t & ccbs, loggerCBs_t & lcbs, const String & stamp) {
  //
  pinMode ((pin = static_cast <uint8_t> (p)), OUTPUT) ;
  digitalWrite (pin, LOW) ;
  //
  //  register control callback (1 x button press -> disable data logging)
  //
  uint8_t (*fsave)(uint8_t) = ccbs.add ([](uint8_t) -> uint8_t {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INFO] rpac::Logger::disable () callback ...") ;
#endif
    //
    enable = false ;
    //
    return 0 ;
    //
  }, 1) ;
  //
#ifdef __DEBUG__LOGGER__
  Serial.print ("[INTERACTIVE] To disable logging enter 'n' or press button within ") ;
  Serial.print (String (waitForCmd / 1000, DEC)) ;
  Serial.println (" sec.") ;
#endif
  //
  {
    //
    typename rpac::Signal <s>::Hook hook (rpac::Signal <s>::scheme::blinkslow) ;
    //
    for (unsigned long mytime = millis () ; mytime + waitForCmd > millis () ; delay (5)) {
      //
      if (!enable) {
        //
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INTERACTIVE] Button has been pressed ...") ;
#endif
        //
        break ;
        //
      }
      //
      if (Serial.available () > 0) {
        //
        int msg = Serial.read () ;
        //
        if (msg == 'n') {
          //
          enable = false ;
          //
          break ;
          //
        }
        //
      }
      //
      rpac::Signal <s>::loop (false) ;
      rpac::Control <b, s>::loop (rpac::Button <b>::loop (), ccbs) ;
      //
    }
    //
  }
  //
  //  restore control callback (1 x button press)
  //
  ccbs.add (fsave, 1) ;
  //
  if (!enable) {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INTERACTIVE] Data logging has been disabled.") ;
    //
    Serial.println (lcbs.headRow (stamp)) ;
#endif
    //
    rpac::Signal <s>::blocking (rpac::Signal <s>::scheme::blinkfast, 6) ;
    //
    mode = 5 ;
    //
    return ;
  }
  //
  //  Digital pin #__ powers the data logger;
  //  Allow the logger to boot before opening
  //  serial connection and awaiting response.
  //
  Serial1.begin (57600) ;
  //
  delay (100) ;
  //
  digitalWrite (pin, HIGH) ;
  //
  delay (300) ;
  //
  for (unsigned long start = millis (), now = start ; now < start + loggerRetryDura; now = millis ()) {
    //
    if (Serial1.available ()) break ;
    //
    delay (100) ;
    //
  }
  //
  if (Serial1.available ()) {
    //
    String msg = Serial1.readString () ;
    //
    msg.trim () ;
    //
    if (msg.indexOf ("2<") != -1) {
      //
      mode = 0 ;
      //
      Serial1.println (lcbs.headRow (stamp)) ;
      Serial1.flush () ;
      //
      //  register control callback (4 x button press -> shutdown)
      //
      ccbs.add ([](uint8_t) -> uint8_t {
        //
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] rpac::Logger::shutdown () callback ...") ;
#endif
        //
        if (mode == 0) mode = 1 ;
        //
        return 0 ;
        //
      }, 4) ;
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logging started per: " + stamp) ;
#endif
      //
      return ;
      //
    } else {
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[WARNING] Logger unit reported error: " + msg) ;
#endif
      //
    }
    //
  } else {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[WARNING] Logger unit not responing.") ;
#endif
    //
  }
  //
#ifdef __DEBUG__LOGGER__
  Serial.println ("[WARNING] Start of data logging failed. Powering off.") ;
#endif
  //
  digitalWrite (pin, LOW) ;
  //
  Serial1.end () ;
  //
  return ;
  //
}
//
bool rpac::Logger::loop (loggerCBs_t & lcbs) {
  //
  unsigned long myTime = millis () ;
  //
  switch (mode) {
    //
    case 0 :
      //
      while (myTime > loggerNextSampleTime) loggerNextSampleTime += loggerSampleInterval ;
      //
      if (myTime < loggerNextSampleTime - loggerSampleAdjust) { return false ; }
      //
      delay (loggerNextSampleTime - myTime) ;
      //
      loggerNextSampleTime += loggerSampleInterval ;
      //
      Serial1.println (lcbs.logRow ("")) ;
      //
      return true ;
      //
    case 1 :
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Logger shutdown - flushing serial connection") ;
#endif
      Serial1.println ("*** end of data log file ***") ;
      Serial1.flush () ;
      loggerShutdownFlushTime = millis () ;
      mode = 2 ;
      //
      break ;
      //
    case 2:
      //
      if (millis () > loggerShutdownFlushTime + 500) {
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] Logger shutdown - terminating serial connection.") ;
#endif
        Serial1.end () ;
        mode = 3 ;
      }
      break ;
      //
    case 3:
      //
      if (millis () > loggerShutdownFlushTime + 1000) {
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] Logger shutdown - switching off logger device.") ;
#endif
        digitalWrite (pin, LOW) ;
        mode = 4 ;
      }
      break ;
      //
    case 4:
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logger shutdown completed.") ;
#endif
      mode = 5 ;
      break ;
      //
    case 5 :
      //
#ifdef __DEBUG__LOGGER__
      //
      while (myTime > loggerNextSampleTime + loggerSampleOutput) loggerNextSampleTime += loggerSampleOutput ;
      //
      if (myTime > loggerNextSampleTime) {
        //
        loggerNextSampleTime += loggerSampleOutput ;
        Serial.println (lcbs.logRow ("")) ;
        //
      }
      //
#endif
      break ;
      //
    default:
#ifdef __DEBUG__LOGGER__
      Serial.println ("[WARNING] Data logger shutdown state error.") ;
#endif
      break ;
  }
  //
  return false ;
  //
}