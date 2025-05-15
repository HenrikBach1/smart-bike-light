#ifndef BATTERY_MONITORING_H
#define BATTERY_MONITORING_H


#include "Pins.h"       
#include "Globals.h"
#include <Arduino.h>

class battery_monitoring {
public:
  void init();

  bool charging();

  int battery_percentage();


  float time_left(int mode);

private:
  // --- ADC and voltage divider parameters ---
  static constexpr float ADC_REF_VOLTAGE       = 3.3F;    
  static constexpr int   ADC_MAX_COUNT         = 4095;   
  static constexpr int   R1                    = 50000;   
  static constexpr int   R2                    = 180000;  

  // --- Battery characteristics ---
  static constexpr float BATTERY_FULL_VOLTAGE      = 4.20F;   // 100 %
  static constexpr float BATTERY_BREAKPOINT_VOLT   = 3.40F;   //  20 %
  static constexpr float BATTERY_CUTOFF_VOLTAGE    = 2.65F;   //   0 %
  static constexpr float BATTERY_CAPACITY_mAh      = 3500.0F; // mAh

  /** returns cell voltage */
  float readCellVoltage();
};

#endif