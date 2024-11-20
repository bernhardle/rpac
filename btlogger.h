//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __btlogger_h_included__
#define __btlogger_h_included__
//
#include "logger.h"
//
namespace rpac {
    //
    class BTLogger : public Logger {
        //
        private :
          //
          static BTLogger * instance ;
          static bool initialized ;
          //
          operator bool () const { return true ; }
          //
        protected:
          //
          BTLogger (loggerCBs_t &cbs) ;
          bool loop (unsigned long int) ;
          void shutdown () { } 
          //
        public:
          //
          static void setup (loggerCBs_t &) ;
          static bool loop () { return Logger::loop () ; }
          static void stop () { if (instance != nullptr) instance->shutdown () ; }
          //
    } ;
    //
} ;
//
#endif
