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


#endif // GLOBALS_H
