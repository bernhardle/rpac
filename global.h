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
constexpr bool __LOG_BY_DEFAULT__ {false} ;
//
typedef class loggerCBs loggerCBs_t ;
//
namespace rpac {
    //
#if defined(ARDUINO_UBLOX_NINA_W10) 
    enum struct Pin {none = 0, logger = 15, button = 27, flow = 33, pressure = A0, pulser = 10, relais = LED_RED, signal = LED_BLUE} ;
#elif defined(ARDUINO_SEEED_XIAO_RP2040)
    enum struct Pin {none = 0u, logger = 2u, button = 4u, flow = 3u, pressure = A0, pulser = 2u, relais = PIN_LED_G, signal = LED_BUILTIN} ;
#elif defined(ARDUINO_ARCH_RP2040)
    enum struct Pin {none = 0u, logger = 2u, button = D12, flow = D20, pressure = A0, pulser = D11, relais = D8, signal = LED_BUILTIN} ;
#else 
    enum struct Pin {none = 0u, logger = 15u, button= 16u, flow = 7u, pressure = A0, pulser = 10u, relais = 2u, signal = LED_BUILTIN} ;
#endif
    //
} ;
//
typedef rpac::Pin rpacPin_t ;
//
#endif
