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
  led_on();
  TX_RETURN_TYPE response = send(MODULE_NONE, STATUS_OK, "!"); // Send data
  if (response == TX_SUCCESS) {
      Serial.println("Message sent!");
  } else if (response == TX_FAIL) {
      Serial.println("Failed to send.");
  } else {
      Serial.println("Unknown response.");
  }
  led_off();
}
