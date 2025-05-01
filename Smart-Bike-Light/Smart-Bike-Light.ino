//file=Smart-Bike-Light.ino

// NOTER:
// Lav routines til start / "slut" af mode/opgaver
//    -- Inde under det her er fx (og vigtigt) WiFi Scan routine + LoRa uplink function (reducer tiden!)
// Tænk over hvad der skal slukkes hvornår
// Lac struct/objekt der holder GLOBAL state af lygte (eller bare brug alle modulerne).
// 

#include "LedControl.h"
#include "CustomLoRa.h"
#include "Globals.h" // Include (Pins.h) for LED_PIN, RST, RX, TX, and UART
#include "Skeleton.h"
#include "WiFiScanner.h" // Include for getTop3Networks function


void setup() {
  Serial.begin(115200); // Open serial communication
  delay(200); // Wait for serial console to open
  

  Serial.printf("\nStartup mode = %d\n", deviceState.mode);

  // Initialize somethings here before startup casue check

  // Since we are in setup, this wakeup cause is from DEEP SLEEP
  switch (esp_sleep_get_wakeup_cause()) {
    case ESP_SLEEP_WAKEUP_UNDEFINED: // First startup
      Serial.println("\tFirst startup");
      firstStartupRoutine();
      break;
    case ESP_SLEEP_WAKEUP_EXT0: // Woken from MPU interrup pin
      Serial.println("\tWoken from MPU interrupt");
      mpuWakeupRoutine();
      break;
    case ESP_SLEEP_WAKEUP_EXT1: // Woken from one of the two buttons
      Serial.println("\tWoken up by button press");
      // Fall through
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("\tWoken up by timer");
      timerWakeupRoutine();
      break;
  }

  
  // Initialize the things we need for ACTIVE mode, which we havents initialized in a previous routine.
  initialize_LED();
  initialize_LoRaWAN();
  initialize_physical_buttons();
  initWiFiScanner();
  Serial.println("WiFi Scanner initialized");

  // This is the last things we do before entering active mode (loop)
  deviceState.mode = ACTIVE;
  Serial.println("\tGoing into active mode");


}


void loop() { // If we reach loop we are in active mode!
  // Read sensors here:
  read_sensors();

  print_info_interval(); // print stuff every "interval" seconds

  lora_ping(); // This will only run every X seconds

  // …any other work you want to do every pass through loop()
  // nothing here will block for a full second
  tick_stuff(); // This is where we set the light brightness, check battery life, movements, (LoRa?) and change modes


}

