//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include <RTClib.h>
//
#include "global.h"
#include "logger.h"
//
rpac::Logger * rpac::Logger::instances [64] {nullptr} ;
template <class A> bool rpac::SerialLogger <A>::initialized {false} ;
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
const char * loggerCBs::logRow (void) {
  //
  char * pos = row ;
  //
  for (int i = 0 ; i < static_cast<int>(num) ; i++) {
    //
    pos += sprintf (pos, (i == 0 ? "%2lu": ";%2lu"), (*cb [i])()) ;
    //
  }
  //
  return row ;
}
//
const char * loggerCBs::headRow (void) {
  //
  char * pos = row ;
  //
  for (int i = 0 ; i < static_cast<int> (num) ; i++) {
    //
    pos += sprintf (pos, (i == 0 ? "%2s" : ";%2s"), hd [i]) ;
    //
  }
  //
  return row ;
}
//
template <class A> void rpac::SerialLogger <A>::shutdown (void) {
  //
#ifdef __DEBUG__LOGGER__
  Serial.println ("[INFO] rpac::Logger::shutdown () ...") ;
#endif
  //
  if (mode == 0) mode = 1 ;
  //
}
//
//
bool rpac::Logger::loop () {
  //
  unsigned long myTime = millis () ;
  //
  bool b {true} ;
  //
  for (int i{0} ; instances [i] != nullptr ; b &= instances [i++]->loop (myTime)) ;
  //
  return b ;
  //
}
//
template <class A> void rpac::SerialLogger <A>::setup (loggerCBs_t & c, serial_t & s, unsigned int a, unsigned int b) {
  //
  if (!initialized) {
    //
    if (new SerialLogger <A> (c, s, a, b)) {
      //
      initialized = true ;
      //
    }
    //
  }
  //
}
//
template <class A> bool rpac::SerialLogger <A>::loop (unsigned long myTime) {
  //
  switch (mode) {
    //
    case 0 :
      //
      return writeLine (myTime) ;
      //
    case 1 :
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Logger shutdown - flushing serial connection") ;
#endif
      log.println ("*** end of data log file ***") ;
      log.flush () ;
      //
      flushTime = millis () ;
      //
      mode = 2 ;
      //
      break ;
      //
    case 2:
      //
      if (millis () > flushTime + 500) {
        //
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] Logger shutdown - terminating serial connection.") ;
#endif
        //
        log.end () ;
        //
        mode = 3 ;
        //
      }
      break ;
      //
    case 3:
      //
      if (millis () > flushTime + 1000) {
        //
#ifdef __DEBUG__LOGGER__
        Serial.println ("[INFO] Logger shutdown - switching off logger device.") ;
#endif
        //
// ***        digitalWrite (pin, LOW) ;
        //
        mode = 4 ;
        //
      }
      break ;
      //
    case 4:
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logger shutdown completed.") ;
#endif
      //
      mode = 5 ;
      //
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
      //
  }
  //
  return false ;
  //
}
//
template <class A> rpac::SerialLogger <A>::SerialLogger (::loggerCBs_t & c, serial_t & s, unsigned int a, unsigned int b) : Logger (c), log(s), loggerSampleInterval {a}, loggerSampleAdjust {b} {
  //
  if (log) {
    //
    mode = 0 ;
    //
    for (int i {0}; Logger::instances [i] != nullptr ? true : (Logger::instances [i] = this , false) ; i++ ) ;
    //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logging to serial port started.") ;
#endif
    //
  } else {
    //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[ERROR] Start of data logging failed.") ;
#endif
    //
  }
  //
}
//
rpac::OpenLogSerialLogger::OpenLogSerialLogger (::loggerCBs_t & l, serial_t & s, rpacPin_t p, unsigned int a, unsigned int b) : SerialLogger <HardwareSerial> (l, s, a, b) {
  //
  pinMode (pin = static_cast <uint8_t> (p), OUTPUT) ;
  digitalWrite (pin, LOW) ;
  //
  //  Digital pin #__ powers the data logger;
  //  Allow the logger to boot before opening
  //  serial connection and awaiting response.
  //
  SerialLogger <HardwareSerial>::log.begin (57600) ;
  //
  delay (100) ;
  //
  digitalWrite (pin, HIGH) ;
  //
  delay (300) ;
  //
  for (unsigned long start = millis (), now = start ; now < start + __retry; now = millis ()) {
    //
    if (SerialLogger <HardwareSerial>::log.available ()) break ;
    //
    delay (100) ;
    //
  }
  //
  if (SerialLogger <HardwareSerial>::log.available ()) {
    //
    String msg = SerialLogger <HardwareSerial>::log.readString () ;
    //
    msg.trim () ;
    //
    if (msg.indexOf ("2<") != -1) {
      //
      SerialLogger <HardwareSerial>::mode = 0 ;
      //
      for (int i{0} ; Logger::instances [i] != nullptr ; Logger::instances [i++]->writeHead ()) ;
      //
      SerialLogger <HardwareSerial>::log.flush () ;
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logging started.") ;
#endif
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
  SerialLogger <HardwareSerial>::log.end () ;
  //
}
//
template <class A> void rpac::SerialLogger <A>::writeHead (void) {
  //
  log.println (Logger::callbacks.headRow ()) ;
  //
}
//
template <class A> bool rpac::SerialLogger <A>::writeLine (unsigned long int myTime) {
  //
  while (myTime > loggerNextSampleTime) loggerNextSampleTime += loggerSampleInterval ;
  //
  if (myTime < loggerNextSampleTime - loggerSampleAdjust) { return false ; }
  //
  delay (loggerNextSampleTime - myTime) ;
  //
  loggerNextSampleTime += loggerSampleInterval ;
  //
  log.println (Logger::callbacks.logRow ()) ;
  //
  return true ;
  //
}
