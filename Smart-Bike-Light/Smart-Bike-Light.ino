//file=Smart-Bike-Light.ino

/*
 * Includes section
 */

#include "LedControl.h"
#include "CustomLoRa.h"
#include "Globals.h" // Include (Pins.h) for LED_PIN, RST, RX, TX, and UART

// Define global application variables
String message = ""; // Global message variable for LoRa communications

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
    
    // Use decompose_message to extract module and data from received message
    DecomposedMessage decomposed = decompose_message(message);
    
    // Print the decomposed message components using cast instead of switch
    Serial.print("To Module: ");
    Serial.println((int)decomposed.toModule);
    
    Serial.print("Data: ");
    Serial.println(decomposed.data);
    
    // Process message based on the module
    if (decomposed.toModule == MODULE_LORAWAN) {  //01
      // Handle LoRaWAN specific commands
      if (decomposed.data == "LED_ON" || decomposed.data == "1") {
        led_on();
        Serial.println("LED turned ON by LoRaWAN command");
      } else if (decomposed.data == "LED_OFF" || decomposed.data == "0") {
        led_off();
        Serial.println("LED turned OFF by LoRaWAN command");
      }
    } else {
      // Handle other modules or unknown modules
      Serial.println("Message for other module or unknown format");
    }
    
    message = "";
    Serial.println("Reset message: " + message);
  }
}
