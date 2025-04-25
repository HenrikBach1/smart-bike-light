//file=Smart-Bike-Light.ino

/*
 * Includes section
 */

#include "LedControl.h"
#include "CustomLoRa.h"
#include "Globals.h" // Include (Pins.h) for LED_PIN, RST, RX, TX, and UART

void setup() {
  Serial.begin(115200); // Open serial communication
  delay(200); // Wait for serial console to open
  Serial.println("Startup");

  initialize_LED();
  initialize_LoRaWAN();
  
  // Join TTN once during setup
  if (join_TTN()) {
    Serial.println("TTN join successful in setup");
  } else {
    Serial.println("TTN join failed in setup, will retry in loop");
  }
}

void loop() {
  led_on();

  // Connection to TTN is now handled automatically in tranceive function
  
  Serial.println("TXing");
  TX_RETURN_TYPE response = tranceive(MODULE_NONE, STATUS_OK, "!"); // Send data and receive waiting data
  if (response == TX_SUCCESS) {
      Serial.println("Message sent!");
  } else if (response == TX_FAIL) {
      Serial.println("Failed to send.");
  } else {
      Serial.println("Unknown response.");
  }

  if (message != "") {
    Serial.println("RXing: " + message);
    message = "";
    Serial.println("Reset message: " + message);
  }

  led_off();
}
