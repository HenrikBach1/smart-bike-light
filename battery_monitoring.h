#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H


class battery_monitoring {

  public:

    int adc_voltage_to_bb_percent(int adc_voltage);
    int read_average_adc_voltage(int adc_pin, int samples);
    bool check_charging(int adc_voltage);
    int check_battery_lifetime();

};

#endif


