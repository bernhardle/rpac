//
#include "QuickPID.h"
//
#include "softpid.h"
//
template <rpacPin_t p> float rpac::SoftPID <p>::sVal ;
template <rpacPin_t p> float rpac::SoftPID <p>::iVal ;
template <rpacPin_t p> float rpac::SoftPID <p>::oVal ;
template <rpacPin_t p> QuickPID rpac::SoftPID <p>::pid (&sVal, &iVal, &oVal) ;
//
template <rpacPin_t p> void rpac::SoftPID <p>::setup (loggerCBs_t & lcbs) {
    //
    rpac::Pressure<p>::setup (lcbs) ;
    //
    lcbs.add ([]() -> unsigned long { return static_cast <unsigned long int> (oVal) ; }, "PID out") ;
    //
}
//
template <rpacPin_t p> void rpac::SoftPID <p>::loop () {
    //
    rpac::Pressure <p>::loop () ;
    //
}