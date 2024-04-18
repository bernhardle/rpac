//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __fslogger_h_included__
#define __fslogger_h_included__
//
#include "logger.h"
//
#define LITTLEFS_FILE_NAME
//
namespace rpac {
    //
    class FlashLogger : public Logger {
        //
        private :
            //
            static bool enable ;
            //
            static void listFolder (const String &) ;
            static long int usage () ;
            //
            uint8_t mode {3u} ;
            //
        protected :
            //
            bool loop (unsigned long int) ;
            void writeHead (void) ;
            bool writeLine (unsigned long int) ;
            //
        public :
            //
            FlashLogger (loggerCBs_t &, unsigned int = 100, unsigned int = 4) ;
            //
            static bool loop () { return Logger::loop () ; }
            //
    } ;
    //
}
//
#endif
