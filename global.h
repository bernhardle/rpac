//
//  (c) Bernhard Schupp, Frankfurt (2024-2025)
//
#ifndef __global_h_included__
#define __global_h_included__
//
#define __DEBUG__RPAC__
//
#ifdef __DEBUG__RPAC__
// #define __DEBUG__BUTTON__
// #define __DEBUG_CONTROL__
// #define __DEBUG__FLOW__
// #define __DEBUG__LOGGER__
// #define __DEBUG__PRESSURE__
#define __DEBUG__PULSER__
// #define __DEBUG__RELAIS__
// #define __DEBUG__RTC__
// #define __DEBUG__SIGNAL__
#else
#warning "Debug info disabled"
#endif
//
constexpr bool __LOG_BY_DEFAULT__ {true} ;
//
typedef class loggerCBs loggerCBs_t ;
//
namespace rpac {
    //
#if defined(ARDUINO_UBLOX_NINA_W10)
#warning "[INFO] u-blox NINA W10 selected"
    enum struct Pin {none = 0u, logger = 15, button = 27, flow = 33, pressure = A0, pulser = 10, relais = LED_RED, signal = LED_BLUE} ;
//
#elif defined(ARDUINO_SEEED_XIAO_RP2040)
#if defined(ARDUINO_ARCH_RP2040)
#warning "[INFO] Seeed Xiao RP2040 based arduino with RBP Pico architecture"
    enum struct Pin {none = 0u, logger = 2u, button = 4u, flow = D2, pressure = A0, pulser = 2u, relais = PIN_LED_G, signal = LED_BUILTIN} ;
#elif defined(ARDUINO_ARCH_MBED)
#warning "[INFO] Seeed Xiao RP2040 based arduino with MBED architecture"
    enum struct Pin {none = 0u, logger = 2u, button = 4u, flow = D2, pressure = A0, pulser = 2u, relais = PIN_LED_G, signal = LED_BUILTIN} ;
#else
#error "[FATAL] Seeed Xiao RP2040 with no known architecture"
#endif
//
#elif defined(ARDUINO_Seeed_XIAO_nRF52840)
#warning "[INFO] Seeed Xiao nRF52840 with native architecture"
    enum struct Pin {none = 0u, logger = D7, button = D9, flow = D8, pressure = A0, pulser = A1, relais = D6, signal = LED_GREEN} ;
//
#elif defined(ARDUINO_SEEED_XIAO_NRF52840)
#warning "[INFO] Seeed Xiao nRF52840 with MBED architecture"
    enum struct Pin {none = 0u, logger = D7, button = D9, flow = D8, pressure = A0, pulser = A1, relais = D6, signal = LED_BUILTIN} ;
//
#elif defined(ARDUINO_NANO_RP2040_CONNECT)
#if defined(ARDUINO_ARCH_RP2040)
#warning "[INFO] RP2040 based arduino with RBP Pico architecture"
    enum struct Pin {none = 0u, logger = 2u, button = 12u, flow = 9u, pressure = A0, pulser = 10u, relais = 8u, signal = LED_BUILTIN} ;
#elif defined(ARDUINO_ARCH_MBED)
#warning "[INFO] RP2040 based arduino with MBED architecture"
    enum struct Pin {none = 0u, logger = D2, button = D12, flow = D9, pressure = A0, pulser = D10, relais = D8, signal = LED_BUILTIN} ;
#else
#error "[FATAL] RP2040 based arduino with no known architecture"
#endif
//
#else
#warning "[WARNING] Using default values for pin numbering"
    enum struct Pin {none = 0u, logger = 15u, button= 16u, flow = 7u, pressure = A0, pulser = 10u, relais = 2u, signal = LED_BUILTIN} ;
#endif
    //
} ;
//
typedef rpac::Pin rpacPin_t ;
//
#endif
