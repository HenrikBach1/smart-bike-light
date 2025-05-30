//file=Smart-Bike-Light.ino


#include "LedControl.h"
#include "CustomLoRa.h"
#include "Globals.h" // Include (Pins.h) for LED_PIN, RST, RX, TX, and UART
#include "Skeleton.h"
#include "WiFiScanner.h" // Include for getTop3Networks function


void setup() {
  Serial.begin(115200); // Open serial communication
  delay(200); // Wait for serial console to open
  
  // Initialize somethings here before startup casue check
  printDeviceState();
  initialize_LED();
  led_off();
  battery.init();


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
  initialize_physical_buttons();
  initAdx();
  initPwrLed();
  initialize_LoRaWAN();
  Serial.println("Start stuff initialized");

  // This is the last things we do before entering active mode (loop)
  deviceState.mode = ACTIVE;
  deviceState.isMoving = true;
  deviceState.light_on = false;
  Serial.println("\tGoing into active mode");
  clear_in_activity_int();
}


void loop() { // If we reach loop we are in active mode!

  // nothing here will block for a full second
  tick_stuff(); // This is where we act on deviceState
  tick_stuff_interval(); // This will only run every 500ms
  print_info_interval(); // print stuff every "interval" seconds

  // lora_ping_routine(); // This will only run every X seconds - i say we dont need this since it does not make any sense.

  

}

