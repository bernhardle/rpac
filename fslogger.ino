//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#if defined(ARDUINO_SEEED_XIAO_RP2040) || defined(ARDUINO_ARCH_RP2040)
//
/*
struct FSInfo {
    size_t totalBytes;
    size_t usedBytes;
    size_t blockSize;
    size_t pageSize;
    size_t maxOpenFiles;
    size_t maxPathLength;
};
*/
#include "fslogger.h"
#include <LittleFS.h>
#include <EEPROM.h>
//
rpac::FlashLogger * rpac::FlashLogger::instance {nullptr} ;
bool rpac::FlashLogger::initialized {false} ;
bool rpac::FlashLogger::enable {false} ;
//
void rpac::FlashLogger::listFolder (const String & folder) {
  //
  if (enable) {
    //
    Dir d = LittleFS.openDir (folder.c_str ()) ;
    //
    if (false) {
      //
      Serial.println ("FATAL: Unable to open folder \'/" LITTLEFS_FILE_NAME "\' for reading.")  ;
      //
    } else {
      //
      Serial.print ("\t----------------------------------------------\n\n") ;
      Serial.println ("\t\tFolder: " + folder) ;
      //
      for (bool b = d.rewind () ; b ; b = d.next ()) {
        //
        Serial.print ("\t\t\td\t.\n") ;
        Serial.print ("\t\t\td\t..\n") ;
        if (d.isFile ()) {
          Serial.print ("\t\t\tf\t") ;
          Serial.print (d.fileName()) ;
          Serial.print ("\t") ;
          Serial.print (d.fileTime ()) ;
          Serial.print ("\t") ;
          Serial.print (d.fileCreationTime ()) ;
          Serial.print ("\t") ;
          Serial.print (d.fileSize ()) ;
        }
        else if (d.isDirectory ()) {
          Serial.print ("\t\t\td\t") ;
          Serial.print (d.fileName()) ;
        }
        //
      }
      //
      Serial.println ("\n\n\t----------------------------------------------\n") ;
      //
    }
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
      return 0 ; // writeLine (myTime) ;
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
rpac::FlashLogger::FlashLogger (loggerCBs_t & c, unsigned int a, unsigned int b) : Logger (c) {
    //
    EEPROM.begin (64) ;
    //
    // for (int i = 0 ; i < 64 ; EEPROM.write (i++, 0x0)) ;
    //
    enable = LittleFS.begin () ;
    //
    if (enable) {
        //
        FSInfo info ;
        //
        if (LittleFS.info (info)) {
          //
          Serial.print ("\t----------------------------------------------\n\n") ;
          Serial.print ("\tFSInfo:\n") ;
          Serial.print ("\t\ttotal: ... ") ;
          Serial.print (String (info.totalBytes / 1024, DEC)) ;
          Serial.print (" kb\n\t\tused: .... ") ;
          Serial.print (String (info.usedBytes / 1024, DEC)) ;
          Serial.print (" kb (") ;
          Serial.print (String (static_cast <float> (info.usedBytes) / static_cast <float> (info.totalBytes + 1),2)) ;
          Serial.println ("%)\n\n\t----------------------------------------------") ;
          //
        }
        //
        listFolder ("/") ;
        //
        mode = 0u ;
        //
    } else {
      //
      Serial.println ("[ERROR] LittleFS mount failed.") ;
      //
    }
    //
    EEPROM.end () ;   // calls .commit() ;
    //
}
//
void rpac::FlashLogger::setup (loggerCBs_t & c, unsigned int a, unsigned int b) {
  //
  if (!initialized) {
    //
    instance = new FlashLogger (c, a, b) ;
    //
    if (*instance) {
      //
      initialized = true ;
      //
    } else {
      //
      Serial.println ("[ERROR] FlashLogger initialization failed.") ;
      //
    }
    //
  }
  //
}
#endif