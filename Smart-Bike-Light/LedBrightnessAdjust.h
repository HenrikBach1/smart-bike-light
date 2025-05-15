//file=LedBrightnessAdjust.h

#ifndef LedAdjustBrightness
#define LedAdjustBrightness

#include <Arduino.h>
#include "Globals.h" // Include for module enable/disable flags
#include "Pins.h" // Include Pins.h for LED_PIN

#if ENABLE_LED_BRIGHTNESS

// Declare global variables needed for brightness adjustment
extern String input;
#define ledPin PWR_LED_PIN  // Use the LED_PIN defined in Pins.h

void initPwrLed();
void adjustBrightness();
void adjustBrightnessSimple(int value);
void shortBlinkLed(int n);

#else

// Empty function stub when module is disabled
inline void initPwrLed() {}
inline void adjustBrightness() {}
inline void adjustBrightnessSimple() {}
inline void shortBlinkLed(int n) {}

#endif // ENABLE_LED_BRIGHTNESS

#endif // LEDADJUSTBRIGHTNESS
