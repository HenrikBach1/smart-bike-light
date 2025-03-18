//file=Network.h

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <rn2xx3.h>
#include "Pins.h" // Include Pins.h for LED_PIN, RST, RX, TX, and UART

typedef enum {
    MODULE_NONE = 0,
    MODULE_LORAWAN
} Module;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1
} Status;

extern HardwareSerial mySerial; // Declare Serial object
extern rn2xx3 myLora;           // Declare rn2xx3 object

void initialize_globals();
void initialize_LoRaWAN();
void initialize_module_rn2483_LoRa();
TX_RETURN_TYPE send(Module module, Status status, const char* data);

#endif // NETWORK_H
