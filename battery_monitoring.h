#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H

const int resolution = 4096;
const int adc_check_voltage = 15;
const int adc_check_charging = 2;
const int interval = 1000*60*10;
const float max_adc_volt = 3.3f;
const float battery_full_volt = 4.2f;
bool charging = false;
unsigned long last_read = 0;
unsigned long current_read = 0;


class battery_monitoring {

  public:

    int adc_voltage_to_bb_percent();
    bool is_charging();
    int check_battery_lifetime();

};

#endif


