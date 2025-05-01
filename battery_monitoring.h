// battery_monitoring.h
#ifndef BATTERY_MONITORING_H
#define BATTERY_MONITORING_H

#include <Arduino.h>

class battery_monitoring {
public:
    /// Konverterer rå ADC-værdi til batteriprocent (0–100)
    int adc_voltage_to_bb_percent();

    /// Returnerer true hvis enheden oplades lige nu
    bool is_charging();

    /// Tjekker batterilevetid (eksempel-placeholder)
    int check_battery_lifetime();

private:
    // --- Konstanter ---
    static constexpr int   resolution = 4096;    // ADC range
    static constexpr int   adc_check_voltage_pin = 15;                    // pin til batteri-måling
    static constexpr int   adc_check_charging_pin = 2;                     // pin til opladnings-detektion
    static constexpr unsigned long interval = 1000UL * 60UL * 10UL; // 10 minutter
    static constexpr float max_adc_volt = 3.3f;                  // referencespænding
    static constexpr float battery_full_volt = 4.2f;                  // fuldt batteri

    // --- Tilstand ---
    bool charging = false;
    unsigned long last_read = 0;
    unsigned long current_read = 0;
};

#endif // BATTERY_MONITORING_H
