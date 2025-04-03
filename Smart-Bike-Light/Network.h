//file=Network.h

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <rn2xx3.h>
#include "Globals.h" // Include Pins.h for LED_PIN, RST, RX, TX, and UART
//HB: #include "Pins.h" // Include Pins.h for LED_PIN, RST, RX, TX, and UART

extern HardwareSerial mySerial; // Declare Serial object
extern rn2xx3 myLora;           // Declare rn2xx3 object

extern String message;

void initialize_globals();
void initialize_LoRaWAN();
void initialize_module_rn2483_LoRa();
TX_RETURN_TYPE transeive(Module module, Status status, const char* data);

#endif // NETWORK_H
