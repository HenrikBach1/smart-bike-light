//file=LedControl.h

#ifndef LedAdjustBrightness
#define LedAdjustBrightness

#include <Arduino.h>
#include "Pins.h" // Include Pins.h for LED_PIN

// Declare global variables needed for brightness adjustment
extern String input;
#define ledPin LED_PIN  // Use the LED_PIN defined in Pins.h

void adjustBrightness();

#endif // LEDADJUSTBRIGHTNESS
