//file=CustomLoRa.h

#ifndef CUSTOMLORA_H
#define CUSTOMLORA_H

#include <Arduino.h>
#include "Globals.h" // Include for module enable/disable flags

#if ENABLE_CUSTOM_LORA

#include <HardwareSerial.h>
#include <rn2xx3.h>
#include "Pins.h" // Explicitly include Pins.h for pin definitions

extern HardwareSerial mySerial; // Declare Serial object
extern rn2xx3 myLora;           // Declare rn2xx3 object

// Use the global message variable from the main sketch
extern String message;

void initialize_globals();
void initialize_LoRaWAN();
bool join_TTN();
bool leave_TTN();
void initialize_module_rn2483_LoRa();
TX_RETURN_TYPE tranceive(Module module, Status status, const char* data);
bool is_joined_TTN();

#else

// Need to include rn2xx3.h for TX_RETURN_TYPE even in function stubs
#include <rn2xx3.h>

// Empty function stubs when module is disabled
inline void initialize_globals() {}
inline void initialize_LoRaWAN() {}
inline bool join_TTN() { return false; }
inline bool leave_TTN() { return false; }
inline void initialize_module_rn2483_LoRa() {}
inline TX_RETURN_TYPE tranceive(Module module, Status status, const char* data) { return TX_FAIL; }
inline bool is_joined_TTN() { return false; }

#endif // ENABLE_CUSTOM_LORA

#endif // CUSTOMLORA_H
