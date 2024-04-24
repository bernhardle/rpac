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
            static bool enable, initialized ;
            //
            static void listFolder (const String &) ;
            static long int usage () ;
            static class FlashLogger * instance ;
            //
            uint8_t mode {3u} ;
            //
        protected :
            //
            bool loop (unsigned long int) ;
            void shutdown (void) {}
            operator bool () const { return mode == 0u ; }
            //
        public :
            //
            FlashLogger (loggerCBs_t &, unsigned int = 100, unsigned int = 4) ;
            //
            static void setup (loggerCBs_t &, unsigned int = 100, unsigned int = 4) ;
            static bool loop () { return Logger::loop () ; }
            static void stop () { if (instance != nullptr) instance->shutdown () ; }
            //
    } ;
    //
}
//
#endif
