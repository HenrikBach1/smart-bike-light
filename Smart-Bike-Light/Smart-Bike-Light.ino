//file=Smart-Bike-Light.ino

/*
 * Includes section
 */

#include "LedControl.h"
#include "Network.h"

void setup() {
  Serial.begin(57600); // Open serial communication
  delay(200); // Wait for serial console to open
  Serial.println("Startup");

  initialize_LED();
  initialize_LoRaWAN();
}

void loop() {
  send(MODULE_NONE, STATUS_OK, "!"); // Send data
}
