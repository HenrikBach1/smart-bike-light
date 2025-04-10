//file=Pins.h

#ifndef PINS_H
#define PINS_H

#include <Arduino.h> // Include Arduino library for digitalWrite and pinMode

#define LED_PIN 2   // GPIO for LED
#define RX      18  // UART RX
#define TX      19  // UART TX
#define RST     23  // Reset pin
#define UART    1   // UART1
#define ADC_PIN 39 // ASBJØRN: Analog input for battery monitoring (checking battery voltage level) 

#endif // PINS_H
