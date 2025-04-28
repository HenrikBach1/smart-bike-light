#include <Arduino.h>
#include "PhotoResistor.h"

/* This module's compilation is controlled by the ENABLE_PHOTO_RESISTOR flag in Globals.h
 * When ENABLE_PHOTO_RESISTOR is set to 0, this implementation code is excluded from compilation
 * When ENABLE_PHOTO_RESISTOR is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in PhotoResistor.h when this module is disabled
 */

#if ENABLE_PHOTO_RESISTOR

PhotoResistor::PhotoResistor(int photoresistorPin, int threshold) {
  _photoresistorPin = photoresistorPin;
  _state = false;
  _threshold = threshold;
}

void PhotoResistor::update() {
  int lightLevel = analogRead(_photoresistorPin);
  if (lightLevel < _threshold) {
    _state = true;
  } else {
    _state = false;
  }
}

bool PhotoResistor::state() {
  return _state;
}

#endif // ENABLE_PHOTO_RESISTOR