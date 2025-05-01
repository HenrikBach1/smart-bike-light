// file=CustomLoRa.h

#ifndef CUSTOMLORA_H
#define CUSTOMLORA_H

#include <Arduino.h>
#include "Globals.h" // Include for module enable/disable flags

#if ENABLE_LORA_MODULE

#include <HardwareSerial.h>
#include <rn2xx3.h>
#include "Pins.h" // Explicitly include Pins.h for pin definitions

extern HardwareSerial myLoRaSerial; // Declare Serial object
extern rn2xx3 myLora;           // Declare rn2xx3 object

// Structure to hold the decomposed message components
struct DecomposedMessage {
    Module toModule;
    String data;
};

// Global decomposed message variable accessible to all modules
extern DecomposedMessage g_decomposedMessage;

void initialize_globals();
void initialize_LoRaWAN();
bool join_TTN();
bool leave_TTN();
void initialize_module_rn2483_LoRa();
TX_RETURN_TYPE tranceive(Module module, Status status, const char* data);
bool is_joined_TTN();
void processLoRaWANMessage(const DecomposedMessage& message);
void wakeUp(); // Function to reset RX pin and send 0x55 character
void deepSleep(); // Function to put RN2483 into deep sleep mode

#else

// Need to include rn2xx3.h for TX_RETURN_TYPE even in function stubs
#include <rn2xx3.h>

// Structure to hold the decomposed message components (even when disabled)
struct DecomposedMessage {
    Module toModule;
    String data;
};

// Global decomposed message variable accessible to all modules (even when disabled)
extern DecomposedMessage g_decomposedMessage;

// Empty function stubs when module is disabled
inline void initialize_globals() {}
inline void initialize_LoRaWAN() {}
inline bool join_TTN() { return false; }
inline bool leave_TTN() { return false; }
inline void initialize_module_rn2483_LoRa() {}
inline TX_RETURN_TYPE tranceive(Module module, Status status, const char* data) { return TX_FAIL; }
inline bool is_joined_TTN() { return false; }
inline void processLoRaWANMessage(const DecomposedMessage& message) {}
inline void wakeUp() {} // Stub for the reset function
inline void deepSleep() {} // Stub for the deep sleep function

#endif // ENABLE_LORA_MODULE

#endif // CUSTOMLORA_H
