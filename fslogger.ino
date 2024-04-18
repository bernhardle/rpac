//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#if defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_ARCH_RP2040)
//
#include "fslogger.h"
#include <LittleFS.h>
#include <EEPROM.h>
//
bool rpac::FlashLogger::enable {false} ;
//
void rpac::FlashLogger::listFolder (const String & folder) {
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
bool rpac::FlashLogger::loop (unsigned long int myTime) {
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
void rpac::FlashLogger::writeHead (void) { }
//
bool rpac::FlashLogger::writeLine (unsigned long int) { return true ; }
//
rpac::FlashLogger::FlashLogger (loggerCBs_t & c, unsigned int a, unsigned int b) : Logger (c) {
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