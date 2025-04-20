//file=Network.h

#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <TheThingsNetwork.h>
#include <rn2xx3.h>
#include "Globals.h" // Include Pins.h for LED_PIN, RST, RX, TX, and UART

extern HardwareSerial mySerial; // Declare Serial object
extern rn2xx3 myLora;           // Declare rn2xx3 object

extern String message;
extern String rawMessage; // Declare the global variable to make it accessible
extern String metadataJson; // Declare the metadata JSON variable

// Public interface functions
const String& getDevEUI(); // Provide a getter function for devEUI
void initialize_globals();
void initialize_LoRaWAN();
TX_RETURN_TYPE tranceive(Module module, Status status, const char* data);
void estimateLocationFromMetadata(const String& metadata);

// Function to retrieve RSSI value from LoRa module
int getRSSI();

#endif // NETWORK_H
