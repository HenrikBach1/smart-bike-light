//file=Smart-Bike-Light.ino

/*
 * Includes section
 */

#include "LedControl.h"
#include "CustomLoRa.h"
#include "Globals.h" // Include (Pins.h) for LED_PIN, RST, RX, TX, and UART
#include "WiFiScanner.h" // Include for getTop3Networks function

// Define global application variables
String message = ""; // Global message variable for LoRa communications

void setup() {
  Serial.begin(115200); // Open serial communication
  delay(200); // Wait for serial console to open
  Serial.println("Startup");

  initialize_LED();
  initialize_LoRaWAN();

  Serial.println("LoRaWAN initialized");  
  initWiFiScanner();
  Serial.println("WiFi Scanner initialized");

  // Join TTN once during setup
  if (join_TTN()) {
    Serial.println("TTN join successful in setup");
  } else {
    Serial.println("TTN join failed in setup, will retry in loop");
  }
}

void loop() {

  // Transmit and receive data
  Serial.println("Sending status update...");
  TX_RETURN_TYPE response = tranceive(MODULE_NONE, STATUS_OK, "!"); 
  
  // Display response status
  if (response == TX_SUCCESS) {
    Serial.println("Status update sent successfully");
  } else {
    Serial.println("Failed to send status update");
  }
  
  // Print out the contents of the global decomposed message
  Serial.println("\n--- BEGIN Global Decomposed Message Contents ---");
  Serial.print("Target Module: ");
  switch (g_decomposedMessage.toModule) {
    case MODULE_NONE:
      Serial.println("None (0)");
      break;
    case MODULE_LORAWAN:
      Serial.println("LoRaWAN (1)");
      break;
    default:
      Serial.print("Unknown (");
      Serial.print((int)g_decomposedMessage.toModule);
      Serial.println(")");
      break;
  }
  
  Serial.print("Message Data: \"");
  Serial.print(g_decomposedMessage.data);
  Serial.println("\"");
  Serial.println("--- END Global Decomposed Message Contents ---\n");
  
  // Add a delay to avoid flooding the serial console
  delay(5000);
}
