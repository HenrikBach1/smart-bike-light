#include <Arduino.h>
#include "PhotoResistor.h"

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