#include <RTClib.h>
//
#include "global.h"
#include "pressure.h"
#include "logger.h"
//
const unsigned long waitForCmd = 5000, loggerFadeOut = 60000, loggerInterval = 50 ;
//
unsigned long loggerLastWrite = 0 ;
unsigned int loggerLastSample = 0 ;
bool loggerEnabled = false ;
//
bool loggerCBs::add (unsigned long (*f)(void)) {
  //
  if (num < mcbs) {
    //
    cb [num ++] = f ;
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
void loggerSetup (String && stamp) {
  //
  bool loggerCmd = true ;
  //
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
      Serial1.println (stamp) ;
      Serial1.flush () ;
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
void loggerLoop (const pressureData_t & data, bool pulserState, bool flowState, String && message, const loggerCBs_t & callbacks) {
  //
  unsigned long myTime = millis () ;
  //
  //
  if (data.sample > loggerLastSample) {
    //
    if (myTime < loggerLastWrite + loggerInterval) {
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[WARNING] Logging frequency limit exceeded: Sample skipped.") ;
#endif
      //
      return ;
      //
    }
    //
    for (int i = 0 ; i < callbacks.num ; i++) {
      //
      Serial.print (String ((*callbacks.cb[i])(), DEC)) ;
      Serial.print (";") ;
      //
    }
    //
    Serial.println (message) ;
    //
    if (! loggerEnabled) return ;
    //
    Serial1.print (String(data.sample, DEC)) ;
    Serial1.print (";") ;
    Serial1.print (String(data.time, DEC)) ;
    Serial1.print (";") ;
    Serial1.print (pulserState) ;
    Serial1.print (";") ;
    Serial1.print (flowState) ;
    Serial1.print (";") ;
    Serial1.print (String (digit2hcPa (data.pressure), 2)) ;
    Serial1.print (";") ;
    Serial1.println (message) ;
    //
    loggerLastWrite = myTime ;
    loggerLastSample = data.sample ;
    //
  } else if (myTime > data.time + loggerFadeOut) {
    //
    Serial1.println ("End of data log.") ;
    Serial1.flush () ;
    //
    delay (500) ;
    //
    digitalWrite (loggerPin, LOW) ;
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INFO] Data logging is disabled.") ;
#endif
    //
    Serial1.end () ;
    //
    loggerEnabled = false ;
    //
  }
  //
}