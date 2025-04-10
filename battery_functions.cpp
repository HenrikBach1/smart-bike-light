#include "battery_monitoring.h"
#include <Arduino.h>

int battery_monitoring::adc_voltage_to_bb_percent(int adc_voltage) {
  if (adc_voltage >= 4079) {
    return 100;
  } else if (adc_voltage >= 3302) {
    return round(((adc_voltage - 2574) / (4079.0 - 2574)) * 80.0 + 20.0);
  } else if (adc_voltage >= 2574) {
    return round(((adc_voltage - 2574) / (4079.0 - 2574)) * 20.0);
  } else {
    return 0;
  }
}

int battery_monitoring::read_average_adc_voltage(int adc_pin, int samples) {
  int sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(adc_pin); // FIXED: should be adc_pin, not pin
    delay(10); // FIXED: added semicolon
  }
  int adc_average = round(sum / (float)samples); // FIXED: cast to float for more accurate rounding
  return adc_average;
}

bool battery_monitoring::check_charging(int adc_voltage) {
  static int last_adc_voltage = 0; // FIXED: needs to be static to persist between calls
  static unsigned long last_check = 0;
  unsigned long time_now = millis();

  if (time_now - last_check >= 1000) { // example: 1000ms = 1 second interval
    if (last_adc_voltage + 50 < adc_voltage) {
      last_adc_voltage = adc_voltage;
      last_check = time_now;
      return true;
    }
    last_adc_voltage = adc_voltage;
    last_check = time_now;
  }
  return false;
}

int battery_monitoring::check_battery_lifetime() {
  // Placeholder function
  return 0;
}
