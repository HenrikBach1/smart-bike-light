//file=Pins.h

#ifndef PINS_H
#define PINS_H

#include <Arduino.h> // Include Arduino library for digitalWrite and pinMode

#define LED_PIN 2   // GPIO for LED
#define RX      18  // UART RX
#define TX      19  // UART TX
#define RST     23  // Reset pin
#define UART    1   // UART1
#define charge_pin 15 // Pin connected to IN+ on charging circuit. Used for battery monitoring
#define adc_pin 4 // Pin connected to B+ on charging circuit. Used for battery monitoring (battery percentage/lifetime)
#endif // PINS_H
