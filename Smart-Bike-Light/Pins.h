//file=Pins.h

#ifndef PINS_H
#define PINS_H

#include <Arduino.h> // Include Arduino library for digitalWrite and pinMode

// Pin definitions for ESP32 board
#define BUTTON_1        27   // GPIO for Button 1
#define BUTTON_2        26   // GPIO for Button 2
#define LED_PIN         2   //Internal LED
#define PWR_LED_PIN     5   // PIN for controlling big led (PWM)
#define ADXL345_INT1    15  // GPIO for ADXL345 Interrupt 1
#define RX              18  // UART RX
#define TX              19  // UART TX
#define SDA             21  // I2C SDA pin
#define SCL             22  // I2C SCL pin
#define LORA_RESET      23  // GPIO for LoRa Reset
#define RST             23  // Same as LORA_RESET for backward compatibility
#define BAT_ADC_INPUT   36  // GPIO for Battery ADC input
#define LDR_ADC_INPUT   39  // GPIO for LDR ADC input
#define UART            1   // UART number (1 or 2) for ESP32
#define BAT_CHG_INPUT   34 // GPIO for USB-charging input


#endif // PINS_H
