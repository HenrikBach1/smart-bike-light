//file=LedControl.cpp

#include <Arduino.h> // Include Arduino library for digitalWrite and pinMode
#include "LedControl.h"
#include "Pins.h" // Explicitly include Pins.h for LED_PIN

/* This module's compilation is controlled by the ENABLE_LED_CONTROL flag in Globals.h
 * When ENABLE_LED_CONTROL is set to 0, this implementation code is excluded from compilation
 * When ENABLE_LED_CONTROL is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in LedControl.h when this module is disabled
 */

#if ENABLE_LED_CONTROL

void initialize_LED() {
    pinMode(LED_PIN, OUTPUT); // Configure LED pin
}

void led_on() {
    digitalWrite(LED_PIN, HIGH); // Use LED_PIN defined in Pins.h
}

void led_off() {
    digitalWrite(LED_PIN, LOW); // Use LED_PIN defined in Pins.h
}

#endif // ENABLE_LED_CONTROL
