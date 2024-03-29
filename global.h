//
//  (c) Bernhard Schupp, Frankfurt (2024)
//
#ifndef __global_h_included__
#define __global_h_included__
//
#define __DEBUG__RPAC__
//
#ifdef __DEBUG__RPAC__
#define __DEBUG__BUTTON__
#define __DEBUG_CONTROL__
#define __DEBUG__FLOW__
#define __DEBUG__LOGGER__
#define __DEBUG__PRESSURE__
#define __DEBUG__PULSER__
#define __DEBUG__RELAIS__
#define __DEBUG__RTC__
#define __DEBUG__SIGNAL__
#endif
//
typedef class loggerCBs loggerCBs_t ;
//
namespace rpac {
    //
#if defined(ARDUINO_UBLOX_NINA_W10) 
    enum struct Pin {none = 0, logger = 15, button = 27, flow = 33, pressure = A0, pulser = 10, relais = LED_RED, signal = LED_BLUE} ;
#elif defined(ARDUINO_SEEED_XIAO_RP2040)
    enum struct Pin {none = 0u, logger = 2u, button = 3u, flow = 4u, pressure = 29u, pulser = 27u, relais = PIN_LED_G, signal = LED_BUILTIN} ;
#else 
    enum struct Pin {none = 0, logger = 15, button= 16, flow = 7, pressure = A0, pulser = 10, relais = 2, signal = LED_BUILTIN} ;
#endif
    //
} ;
//
typedef rpac::Pin rpacPin_t ;
//
#endif
