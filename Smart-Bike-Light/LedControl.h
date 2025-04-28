//file=LedControl.h

#ifndef LEDCONTROL_H
#define LEDCONTROL_H

#include "Globals.h" // Include for module enable/disable flags

#if ENABLE_LED_CONTROL

void initialize_LED();
void led_on();
void led_off();

#else

// Empty function stubs when module is disabled
inline void initialize_LED() {}
inline void led_on() {}
inline void led_off() {}

#endif // ENABLE_LED_CONTROL

#endif // LEDCONTROL_H
