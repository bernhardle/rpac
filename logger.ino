#include <RTClib.h>
//
#include "global.h"
#include "button.h"
#include "logger.h"
//
const unsigned long waitForCmd = 5000, loggerSampleInterval = 100, loggerSampleAdjust = 4 ;
//
static unsigned long loggerNextSampleTime = 0, loggerShutdownFlushTime = 0 ;
static int loggerShutDownStage = 0 ;
static bool loggerEnabled = false ;
static pin_size_t loggerPin ;
//
static uint8_t loggerControlCB (void) {
  //
  Serial.println ("[INFO] loggerControlCB () for shutdown ...") ;
  //
  if (loggerEnabled) loggerShutDownStage = 1 ;
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
#ifdef __DEBUG__PRESSURE__
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
static void loggerBlinkFast (void) {
  //
  for (int i = 0; i < 20; i++) {
    //
    digitalWrite (LED_BUILTIN, HIGH);
    delay (10) ;
    digitalWrite (LED_BUILTIN, LOW);
    delay (50) ;
    //
  }
}
//
static inline float digit2hcPa (int dgs) {
  //  
  //  Voltage divider is 100 k / 147 k
  //  in order to scale max. out < 3.3V
  //  pressure zero offset is ~ 33 digits raw
  //  MXP555 sensitvity is 9.0 mV / kPa
  //  ADC conversion rate is 5 / 1.024 mV / digit 
  //
  static const float sca = float (1.47 * 5.0/1024 / 0.9) ;
  //
  return float (max (dgs - 33, 0)) * sca ;
  //
}
//
void loggerSetup (pin_size_t pin, controlCBs_t & ccbs, loggerCBs_t & lcbs, const String & stamp) {
  //
  bool loggerCmd = true ;
  //
  loggerPin = pin ;
  pinMode (loggerPin, OUTPUT) ;
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
  for (unsigned long mytime = millis () ; mytime + waitForCmd > millis () ; delay (200)) {
    //
    digitalWrite (LED_BUILTIN, HIGH) ;
    //
    if (buttonPressedTime > 0) {
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INTERACTIVE] Button has been pressed ...") ;
#endif
      //
      loggerBlinkFast () ;
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
        loggerBlinkFast () ;
        //
        loggerCmd = false ;
        //
        break ;
        //
      }
      //
    }
    //
    delay (150) ;
    //
    digitalWrite (LED_BUILTIN, LOW) ;
    //
    delay (450) ;
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
      loggerEnabled = true ;
      //
      Serial1.println (lcbs.headRow (stamp)) ;
      Serial1.flush () ;
      //  register shutdown callback for 3 x button press
      ccbs.add (loggerControlCB, 3) ;
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
bool loggerLoop (const String & message, loggerCBs_t & lcbs) {
  //
  unsigned long myTime = millis () ;
  //
  switch (loggerShutDownStage) {
    //
    case 0 :
      //
      break ;
      //
    case 1 :
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Logger shutdown - flushing serial connection") ;
#endif
      Serial1.println ("*** end of data log file ***") ;
      Serial1.flush () ;
      loggerShutdownFlushTime = millis () ;
      loggerShutDownStage = 2 ;
      loggerEnabled = false ;
      break ;
      //
    case 2:
      //
      if (millis () > loggerShutdownFlushTime + 500) {
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] Logger shutdown - terminating serial connection.") ;
#endif
        Serial1.end () ;
        loggerShutDownStage = 3 ;
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
        loggerShutDownStage = 4 ;
      }
      break ;
      //
    case 4:
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logger shutdown completed.") ;
#endif
      loggerShutDownStage = 5 ;
      break ;
      //
    case 5 :
      //
      break ;
      //
    default:
#ifdef __DEBUG__LOGGER__
      Serial.println ("[WARNING] Data logger shutdown state error.") ;
#endif
      break ;
  }
  //
  //
  while (myTime > loggerNextSampleTime) loggerNextSampleTime += loggerSampleInterval ;
  //
  if (myTime < loggerNextSampleTime - loggerSampleAdjust) { return false ; }
  //
  delay (loggerNextSampleTime - myTime) ;
  //
  loggerNextSampleTime += loggerSampleInterval ;
  //
  if (loggerEnabled) {
    //
    Serial1.println (lcbs.logRow (message)) ;
    //
    return true ;
    //
  } 
  //
#ifdef __DEBUG__LOGGER__
  //
  Serial.println (lcbs.logRow (message)) ;
  //
#endif
  //
  return false ;
}