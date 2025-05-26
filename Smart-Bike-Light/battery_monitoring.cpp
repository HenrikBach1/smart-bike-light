#include "battery_monitoring.h"
#include <Arduino.h>

void battery_monitoring::init() {
  // Configure charging pin and ADC pin
  pinMode(BAT_CHG_INPUT, INPUT_PULLDOWN);
  pinMode(BAT_ADC_INPUT, INPUT_PULLDOWN);
  analogReadResolution(12);
}
bool battery_monitoring::charging() {
  return digitalRead(BAT_CHG_INPUT) == HIGH; // Returns true if battery is charging

}

float battery_monitoring::readCellVoltage() {
  int raw = analogRead(BAT_ADC_INPUT);

  float vDivider = raw * (ADC_REF_VOLTAGE / float(ADC_MAX_COUNT)); // calculate the voltage corresponding to the raw ADC value

  return vDivider * ((R1 + R2) / float(R2)); // Calculate the cell voltage before the voltage divider
}

int battery_monitoring::battery_percentage() {
  float v = readCellVoltage();
// Returning 100 or 0 percent if the battery level is out of range
  if (v >= BATTERY_FULL_VOLTAGE)    return 100;
  if (v <= BATTERY_CUTOFF_VOLTAGE)  return   0;

  // Returns percentage based on the linear mapping between  [3.4..4.2 V] → [20..100 %]
  if (v >= BATTERY_BREAKPOINT_VOLT) {
    float pct = 20.0F
              + (v - BATTERY_BREAKPOINT_VOLT)
                / (BATTERY_FULL_VOLTAGE - BATTERY_BREAKPOINT_VOLT)
                * 80.0F;
    return int(pct + 0.5F);
  }
  // Returns percentage based on the linear mapping between  [2.65..3.5 V] → [0..20 %]
  else {
    float pct = (v - BATTERY_CUTOFF_VOLTAGE)
              / (BATTERY_BREAKPOINT_VOLT - BATTERY_CUTOFF_VOLTAGE)
                * 20.0F;
    return int(pct + 0.5F);
  }
}

float battery_monitoring::time_left(int mode) {
  float load_mA;
  // Chose load based on which mode light mode the battery is in
  switch (mode) {
    case NO_LIGHT: load_mA = 0.0F; break;
    case ECO:  load_mA = 50.0F;  break;
    case MEDIUM:  load_mA = 100.0F; break;
    case STRONG: load_mA = 200.0F; break;
    default: load_mA = 200.0F; break;
  }

  int   pct      = battery_percentage();
  float rem_mAh  = (pct / 100.0F) * BATTERY_CAPACITY_mAh; // Calculate the remaining capacity

  if (load_mA <= 0.0F) return 9999.0F;
  return rem_mAh / load_mA;  // divides with the load for the light mode, to get an estimate of the battery lifetime
}
