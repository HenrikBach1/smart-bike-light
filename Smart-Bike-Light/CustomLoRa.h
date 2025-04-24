//file=CustomLoRa.h

#ifndef CUSTOMLORA_H
#define CUSTOMLORA_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <rn2xx3.h>
#include "Globals.h" // Include Pins.h for LED_PIN, RST, RX, TX, and UART
#include "Pins.h" // Explicitly include Pins.h for pin definitions

extern HardwareSerial mySerial; // Declare Serial object
extern rn2xx3 myLora;           // Declare rn2xx3 object

extern String message;

void initialize_globals();
void initialize_LoRaWAN();
bool join_TTN();
bool leave_TTN();
void initialize_module_rn2483_LoRa();
TX_RETURN_TYPE tranceive(Module module, Status status, const char* data);
bool is_joined_TTN();

#endif // CUSTOMLORA_H
