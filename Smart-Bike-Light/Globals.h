//file=Globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Pins.h"    // Include Pins.h for LED_PIN

typedef enum {
    MODULE_NONE = 0,
    MODULE_LORAWAN
} Module;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1
} Status;

typedef enum {
  STORAGE,
  PARK,
  ACTIVE
} Mode;


#define UART    1   // UART1
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_PARKED  60    // Wakeup interval in parked mode
#define TIME_TO_SLEEP_STORAGE  -1  // Maybe we should not have timer on storage mode


#endif // GLOBALS_H
