#include "LedControl.h"

void initialize_LED() {
    pinMode(LED_PIN, OUTPUT); // Configure LED pin
}

void led_on() {
    digitalWrite(LED_PIN, HIGH); // Use LED_PIN defined in Pins.h
}

void led_off() {
    digitalWrite(LED_PIN, LOW); // Use LED_PIN defined in Pins.h
}
