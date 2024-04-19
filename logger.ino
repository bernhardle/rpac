//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include <RTClib.h>
//
#include "global.h"
#include "logger.h"
//
rpac::Logger * rpac::Logger::instances [64] {nullptr} ;
template <class A> rpac::SerialLogger <A> * rpac::SerialLogger <A>::instance {nullptr} ;
template <class A> bool rpac::SerialLogger <A>::initialized {false} ;
rpac::OpenLogSerialLogger * rpac::OpenLogSerialLogger::instance {nullptr} ;
bool rpac::OpenLogSerialLogger::initialized {false} ;
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
const char * loggerCBs::data (void) {
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
const char * loggerCBs::head (void) {
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
rpac::Logger::Logger (loggerCBs_t & c) : callbacks(c) {
  //
  for (int i {0}; instances [i] != nullptr ? true : (instances [i] = this , false) ; i++ ) ; 
  //
}
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
    instance = new SerialLogger <A> (c, s, a, b) ;
    //
    if (*instance) {
      //
      initialized = true ;
      //
    }
    //
  }
  //
}
//
template <class A> void rpac::SerialLogger <A>::shutdown (void) {
  //
#ifdef __DEBUG__LOGGER__
  Serial.println ("[INFO] rpac::SeriaLogger::shutdown () ...") ;
#endif
  //
  if (mode == 0) mode = 1 ;
  //
}
//
template <class A> bool rpac::SerialLogger <A>::loop (unsigned long myTime) {
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
      log.println (Logger::dataLine ()) ;
      //
      return true ;
      //
    case 1 :
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Shutdown - flushing serial connection") ;
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
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INFO] Data logging to serial port started.") ;
#endif
    log.println (Logger::headLine ()) ;
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
void rpac::OpenLogSerialLogger::setup (loggerCBs_t & c, serial_t & s, rpacPin_t p, unsigned int a, unsigned int b) {
  //
  if (!initialized) {
    //
    instance = new OpenLogSerialLogger (c, s, p, a, b) ;
    //
    if (*instance) {
      //
      initialized = true ;
      //
    }
    //
  }
  //
}
//
void rpac::OpenLogSerialLogger::stop (void) { 
  //
#ifdef __DEBUG__LOGGER__
  Serial.println ("[INFO] OpenLog data logging stop initiated ...") ;
#endif
  //
  if (instance != nullptr) instance->shutdown () ;
  //
}
//
rpac::OpenLogSerialLogger::OpenLogSerialLogger (::loggerCBs_t & l, serial_t & s, rpacPin_t p, unsigned int a, unsigned int b) {
  //
  pinMode (pin = static_cast <uint8_t> (p), OUTPUT) ;
  digitalWrite (pin, LOW) ;
  //
  //  Digital pin #__ powers the data logger;
  //  Allow the logger to boot before opening
  //  serial connection and awaiting response.
  //
  s.begin (57600) ;
  //
  delay (100) ;
  //
  digitalWrite (pin, HIGH) ;
  //
  delay (300) ;
  //
  for (unsigned long start = millis (), now = start ; now < start + __retry; now = millis ()) {
    //
    if (s.available ()) break ;
    //
    delay (100) ;
    //
  }
  //
  if (s.available ()) {
    //
    String msg = s.readString () ;
    //
    msg.trim () ;
    //
    if (msg.indexOf ("2<") != -1) {
      //
      SerialLogger <HardwareSerial> ::setup (l, s, a, b) ;
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] OpenLog data logging started.") ;
#endif
      //
    } else {
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[WARNING] OpenLog data logger reported error: " + msg) ;
#endif
      //
    }
    //
  } else {
    //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[ERROR] OpenLog data logger not responing.") ;
#endif
    //
  }
  //
#ifdef __DEBUG__LOGGER__
  Serial.println ("[ERROR] Start of data logging failed. Powering off OpenLog data logger.") ;
#endif
  //
  digitalWrite (pin, LOW) ;
  //
  s.end () ;
  //
}
//
void rpac::OpenLogSerialLogger::shutdown (void) {
  //
#ifdef __DEBUG__LOGGER__
    Serial.println ("[INFO] OpenLog data logger shutdown.") ;
#endif
  SerialLogger <HardwareSerial>::stop () ;
  // ***        digitalWrite (pin, LOW) ;
  //
}

