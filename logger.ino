//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#include <RTClib.h>
//
#include "global.h"
#include "logger.h"
//
//template <class A, int n> byte rpac::SerialLogger <A, n>::space [sizeof(std::reference_wrapper <A>)] {0} ;
template <class A, int n> std::reference_wrapper <A> * rpac::SerialLogger <A, n>::log {nullptr} ;
template <class A, int n> rpac::SerialLogger <A, n> * rpac::SerialLogger <A, n>::instances [n] {nullptr} ;
template <class A, int n> unsigned long rpac::SerialLogger <A, n>::flushTime {0U} ;
template <class A, int n> uint8_t rpac::SerialLogger <A, n>::mode {5U} ;
template <class A, int n> uint8_t rpac::SerialLogger <A, n>::pin {static_cast <uint8_t> (rpacPin_t::none)} ;
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
template <class A, int n> void rpac::SerialLogger <A, n>::shutdown (void) {
  //
#ifdef __DEBUG__LOGGER__
  Serial.println ("[INFO] rpac::Logger::shutdown () ...") ;
#endif
  //
  if (mode == 0) mode = 1 ;
  //
}
//
template <class A, int n> bool rpac::SerialLogger <A, n>::setup (serial_t & s) {
  //
  log = new std::reference_wrapper <A> (s) ;
  //
  if (log->get()) {
    //
    mode = 0 ;
    //
    log->get().println ("Switched mode -> 0") ;
    //
    return true ;
    //
  } else {
    //
    return false ;
    //
  }
  //
}
//
template <class A, int n> bool rpac::SerialLogger <A, n>::setup (rpacPin_t p, serial_t & s) {
  //
  log = new std::reference_wrapper <A> (s) ;
  //
  pinMode (pin = static_cast <uint8_t> (p), OUTPUT) ;
  digitalWrite (static_cast <uint8_t> (pin), LOW) ;
  //
  //  Digital pin #__ powers the data logger;
  //  Allow the logger to boot before opening
  //  serial connection and awaiting response.
  //
  log->get().begin (57600) ;
  //
  delay (100) ;
  //
  digitalWrite (pin, HIGH) ;
  //
  delay (300) ;
  //
  for (unsigned long start = millis (), now = start ; now < start + __retry; now = millis ()) {
    //
    if (log->get().available ()) break ;
    //
    delay (100) ;
    //
  }
  //
  if (log->get().available ()) {
    //
    String msg = log->get().readString () ;
    //
    msg.trim () ;
    //
    if (msg.indexOf ("2<") != -1) {
      //
      mode = 0 ;
      //
      for (int i{0} ; instances [i] != nullptr ; instances [i++]->writeHead ()) ;
      //
      log->get().flush () ;
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Data logging started.") ;
#endif
      //
      return true ;
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
  log->get().end () ;
  //
  return false ;
  //
}
//
template <class A, int n> bool rpac::SerialLogger <A, n>::loop (::loggerCBs_t & lcbs) {
  //
  unsigned long myTime = millis () ;
  //
  switch (mode) {
    //
    case 0 :
      //
      {
        //
        bool b{true} ;
        //
        for (int i{0} ; instances [i] != nullptr ; b &= instances [i++]->writeLine (myTime)) ;
        //
        return b ;
        //
      }
      //
    case 1 :
      //
#ifdef __DEBUG__LOGGER__
      Serial.println ("[INFO] Logger shutdown - flushing serial connection") ;
#endif
      log->get().println ("*** end of data log file ***") ;
      log->get().flush () ;
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
        log->get().end () ;
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
        digitalWrite (pin, LOW) ;
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
template <class A, int n> rpac::SerialLogger <A, n>::SerialLogger (::loggerCBs_t & l, unsigned int a, unsigned int b) : callbacks (l), loggerSampleInterval{a}, loggerSampleAdjust{b} {
  //
  for (int i {0}; instances [i] != nullptr ? true : (instances [i] = this , false) ; i++ ) ;
  //
}
//
template <class A, int n> void rpac::SerialLogger <A, n>::writeHead (void) {
  //
  log->get().println (callbacks.headRow ()) ;
  //
}
//
template <class A, int n> bool rpac::SerialLogger <A, n>::writeLine (unsigned long int myTime) {
  //
  while (myTime > loggerNextSampleTime) loggerNextSampleTime += loggerSampleInterval ;
  //
  if (myTime < loggerNextSampleTime - loggerSampleAdjust) { return false ; }
  //
  delay (loggerNextSampleTime - myTime) ;
  //
  loggerNextSampleTime += loggerSampleInterval ;
  //
  log->get().println (callbacks.logRow ()) ;
  //
  return true ;
  //
}
