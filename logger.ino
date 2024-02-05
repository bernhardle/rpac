#include <RTClib.h>
//
#include "global.h"
#include "button.h"
#include "logger.h"
#include "signal.h"
//
const unsigned long int waitForCmd = 3000, loggerSampleInterval = 100, loggerSampleAdjust = 4 ;
#ifdef __DEBUG__LOGGER__
const unsigned long int loggerSampleOutput = 10 * loggerSampleInterval ;
#endif
//
static unsigned long loggerNextSampleTime = 0, loggerShutdownFlushTime = 0 ;
static uint8_t loggerMode = 5U ;
static uint8_t loggerPin ;
//
static uint8_t loggerControlCB (uint8_t) {
  //
  Serial.println ("[INFO] loggerControlCB () for shutdown ...") ;
  //
  if (loggerMode == 0) loggerMode = 1 ;
  //
  return 0 ;
  //
}
//
bool loggerCBs::add (unsigned long (*f)(void), const String & h) {
  //
  if (num < mcbs) {
    //
    cb [num] = f ;
    //
    strcpy (hd [num], h.substring(0,mhdl).c_str ()) ;
    //
    num ++ ;
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
  for (int i = 0 ; i < num ; i++) {
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
  for (int i = 0 ; i < num ; i++) {
    //
    pos += sprintf (pos, "%2s;", hd [i]) ;
    //
  }
  //
  sprintf (pos, "%s", stamp.substring (0,48).c_str()) ;
  //
  return row ;
}
//
void loggerSetup (rpacPin_t pin, controlCBs_t & ccbs, loggerCBs_t & lcbs, const String & stamp) {
  //
  bool loggerCmd = true ;
  //
  pinMode ((loggerPin = static_cast <uint8_t> (pin)), OUTPUT) ;
  digitalWrite (loggerPin, LOW) ;
  //
  buttonPressedTime = 0 ;
  //
  if (loggerFlag) {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INFO] Data logging has been enabled by compiled configuration.") ;
#endif
    //
  } else {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INFO] Data logging has been disabled by compiled configuration.") ;
#endif
    //
    return ;
  }
  //
#ifdef __DEBUG__LOGGER__
  Serial.print ("[INTERACTIVE] To disable logging enter 'n' or press button within ") ;
  Serial.print (String (waitForCmd / 1000, DEC)) ;
  Serial.println (" sec.") ;
#endif
  //
  {
    //
    SignalHook hook (sig::scheme::blinkfast) ;
    //
    for (unsigned long mytime = millis () ; mytime + waitForCmd > millis () ; delay (200)) {
      //
      if (buttonPressedTime > 0) {
        //
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INTERACTIVE] Button has been pressed ...") ;
#endif
        //
        loggerCmd = false ;
        //
        buttonPressedTime = 0 ;
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
          loggerCmd = false ;
          //
          break ;
          //
        }
        //
      }
      //
      hook () ;
      //
    }
    //
  }
  //
  if (!loggerCmd) {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INTERACTIVE] Data logging has been disabled.") ;
    //
    Serial.println (lcbs.headRow (stamp)) ;
#endif
    //
    signalLaunchBlocking (sig::scheme::blinkfast, 7) ;
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
  digitalWrite (loggerPin, HIGH) ;
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
      loggerMode = 0 ;
      //
      Serial1.println (lcbs.headRow (stamp)) ;
      Serial1.flush () ;
      //
      //  register control callback (4 x button press -> shutdown)
      //
      ccbs.add (loggerControlCB, 4) ;
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
  digitalWrite (loggerPin, LOW) ;
  //
  Serial1.end () ;
  //
  return ;
  //
}
//
bool loggerLoop (loggerCBs_t & lcbs) {
  //
  unsigned long myTime = millis () ;
  //
  switch (loggerMode) {
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
      loggerMode = 2 ;
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
        loggerMode = 3 ;
      }
      break ;
      //
    case 3:
      //
      if (millis () > loggerShutdownFlushTime + 1000) {
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] Logger shutdown - switching off logger device.") ;
#endif
        digitalWrite (loggerPin, LOW) ;
        loggerMode = 4 ;
      }
      break ;
      //
    case 4:
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logger shutdown completed.") ;
#endif
      loggerMode = 5 ;
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