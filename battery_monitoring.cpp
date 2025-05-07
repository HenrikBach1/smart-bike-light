#include "battery_monitoring.h"
#include <Arduino.h>

void battery_monitoring::init() {
  // Konfigurer ladestatus‐pin og ADC-pin
  pinMode(BAT_CHG_INPUT, INPUT_PULLDOWN);
  pinMode(BAT_ADC_INPUT, INPUT_PULLDOWN);
  analogReadResolution(12);
}

bool battery_monitoring::charging() {
  return digitalRead(BAT_CHG_INPUT) == HIGH;
}

float battery_monitoring::readCellVoltage() {
  int raw = analogRead(BAT_ADC_INPUT);

  // 2) Omregn rå ADC til spænding på divider-benet
  float vDivider = raw * (ADC_REF_VOLTAGE / float(ADC_MAX_COUNT));

  // 3) Genskab cellespænding før divider
  return vDivider * ((R1 + R2) / float(R2));
}

int battery_monitoring::battery_percentage() {
  float v = readCellVoltage();

  // Clamp opad/nedad
  if (v >= BATTERY_FULL_VOLTAGE)    return 100;
  if (v <= BATTERY_CUTOFF_VOLTAGE)  return   0;

  // Øvre interval [3.4..4.2 V] → [20..100 %]
  if (v >= BATTERY_BREAKPOINT_VOLT) {
    float pct = 20.0F
              + (v - BATTERY_BREAKPOINT_VOLT)
                / (BATTERY_FULL_VOLTAGE - BATTERY_BREAKPOINT_VOLT)
                * 80.0F;
    return int(pct + 0.5F);
  }
  // Nedre interval [2.65..3.4 V] → [0..20 %]
  else {
    float pct = (v - BATTERY_CUTOFF_VOLTAGE)
              / (BATTERY_BREAKPOINT_VOLT - BATTERY_CUTOFF_VOLTAGE)
                * 20.0F;
    return int(pct + 0.5F);
  }
}

float battery_monitoring::time_left(int mode) {
  // Vælg load i mA baseret på mode
  float load_mA;
  switch (mode) {
    case 0:  load_mA = 50.0F;  break;
    case 1:  load_mA = 100.0F; break;
    default: load_mA = 200.0F; break;
  }

  int   pct      = battery_percentage();
  float rem_mAh  = (pct / 100.0F) * BATTERY_CAPACITY_mAh;

  if (load_mA <= 0.0F) return 0.0F;
  return rem_mAh / load_mA;  // timer
}
