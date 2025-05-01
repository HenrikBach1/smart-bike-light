// battery_monitoring.cpp
#include "battery_monitoring.h"
#include <Arduino.h>
#include <cmath>  // for roundf()

int battery_monitoring::adc_voltage_to_bb_percent() {
    int adc_voltage = analogRead(adc_check_voltage);
    // 1) Spænding efter pulldown (post-divider)
    constexpr float R1 = 50000.0f, R2 = 180000.0f;
    float v_out = (float(adc_voltage) / resolution) * max_adc_volt;

    // 2) Genskab batterispænding før divider (pre-divider)
    float v_batt = v_out * (R1 + R2) / R2;

    // 3) Clamp til [0..100] %
    if (v_batt >= battery_full_volt)    return 100;
    if (v_batt <= 3.0f)                 return   0;

    // 4) Lineær mapping fra [3.0..4.2] V til [0..100] %
    float pct = (v_batt - 3.0f) / (battery_full_volt - 3.0f) * 100.0f;
    return int(roundf(pct));
}

bool battery_monitoring::is_charging() {
    // ADC-threshold for fuldt batteri via spændingsdeler
    constexpr float R1 = 100000.0f, R2 = 68000.0f;
    int threshold_adc = int((battery_full_volt * (R2 / (R1 + R2)))
                            / max_adc_volt * resolution + 0.5f);

    int adc = analogRead(adc_check_charging_pin);
    if (adc >= threshold_adc) {
        charging = true;
        Serial.println("Device is charging!");
        return true;
    }
    charging = false;
    return false;
}
