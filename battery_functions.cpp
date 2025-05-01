#include "battery_monitoring.h"
#include <Arduino.h>


int battery_monitoring::adc_voltage_to_bb_percent(int adc_voltage) {
  int R1 = 50000;
  int R2 = 180000;

  float voltage_post_divider = (adc_voltage/resolution)*max_adc_volt;
  float voltage_pre_divider = voltage_post_divider*(R1+R2)/R2;

  if (voltage_pre_divider >= 4079) {
    return 100;
  } else if (adc_voltage >= 3.4) {
    return round(((adc_voltage - 2574) / (4079.0 - 2574)) * 80.0 + 20.0);
  } else if (adc_voltage >= 2.7) {
    return round(((adc_voltage - 2574) / (4079.0 - 2574)) * 20.0);
  } else {
    return 0;
  }
}

bool battery_monitoring::is_charging() {
  const int R1 = 100000;
  const int R2 = 68000;
  const int charging_threshold_adc = int((battery_full_volt*(R2/(R1+R2))/max_adc_volt)*resolution+0.5f);
  int current_charging_adc = analogRead(adc_check_charging);
  if (current_charging_adc >= charging_threshold_adc) {
    charging = true;
    print("Device is charging!");
    return charging;
  }
}

