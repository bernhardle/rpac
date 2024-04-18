//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#if defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_ARCH_RP2040)
//
#include "fslogger.h"
#include <LittleFS.h>
#include <EEPROM.h>
//
template <int n> bool rpac::FlashLogger <n>::enable {false} ;
//
template <int n> void rpac::FlashLogger <n>::listFolder (const String & folder) {
  //
  Dir d = LittleFS.openDir (folder.c_str()) ;
  //
  if (false) {
    //
    Serial.println ("FATAL: Unable to open folder \'/" LITTLEFS_FILE_NAME "\' for reading.")  ;
    //
  } else {
    //
    Serial.println ("\tFolder: " + folder) ;
    //
  }
  //
}
//
template <int n> bool rpac::FlashLogger <n>::loop (unsigned long int myTime) {
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
      // ("*** end of data log file ***") ;
      //  flush () ;
      mode = 2u ;
      //
      break ;
      //
    case 3 :
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
template <int n> void rpac::FlashLogger <n>::writeHead (void) { }
//
template <int n> bool rpac::FlashLogger <n>::writeLine (unsigned long int) { return true ; }
//
template <int n> rpac::FlashLogger <n>::FlashLogger (loggerCBs_t & c, unsigned int a, unsigned int b) : Logger <n> (c) {
    //
    EEPROM.begin (64) ;
    //
    enable = LittleFS.begin () ;
    //
    if (enable) {
        //
        FSInfo info ;
        //
        LittleFS.info (info) ;
        //
        listFolder ("/") ;
    }
    //
    EEPROM.end () ;   // calls .commit() ;
    //
}
//
#endif