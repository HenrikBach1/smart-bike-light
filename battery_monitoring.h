#ifndef MYFUNCTIONS_H
#define MYFUNCTIONS_H


class battery_monitoring {

  public:

    int adc_voltage_to_bb_percent();
    bool is_charging();
    int check_battery_lifetime();

};

#endif


