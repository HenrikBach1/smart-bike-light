
// Skeleton.cpp
#include "Skeleton.h"


unsigned long lastPrintTime = 0;
unsigned long lastPingTime = 0;
const unsigned long printInterval = 5000;  // milliseconds
const unsigned long printIntervalLoRa = 30000;  // milliseconds
RTC_DATA_ATTR Mode mode = STORAGE;

OneButton button1(BUTTON_1, false); /* active-HIGH */
OneButton button2(BUTTON_2, false);

uint8_t lastMacs[3][6] = {{0x00}};
uint8_t lastRssis[3] = {0x00};

DeviceState deviceState = {
  .isMoving = false,
  .isDark = false,
  .isCharging = false,
  .isConnectedToTTN = false,
  .batteryPercentage = 100,
  .batteryTimeLeft = 0,
  .mode = STORAGE,
  .lightMode = ECO
};

// helper to turn enums into human-readable strings:
const char* modeToString(Mode m) {
  switch(m) {
    case STORAGE:      return "STORAGE";
    case PARK:  return "PARK";
    case ACTIVE:  return "ACTIVE";
    // …add other modes here…
    default:           return "UNKNOWN";
  }
}
const char* lightToString(LightMode l) {
  switch(l) {
    case ECO:    return "ECO";
    case MEDIUM: return "NORMAL";
    case STRONG:  return "STRONG";
    // …add other light modes here…
    default:     return "UNKNOWN";
  }
}

// your “print all fields” function:
void printDeviceState() {
  Serial.println(F("---- Device State ----"));
  Serial.print    (F("isMoving:         "));
  Serial.println (deviceState.isMoving ? "YES" : "no");
  Serial.print    (F("isDark:           "));
  Serial.println (deviceState.isDark   ? "YES" : "no");
  Serial.print    (F("isCharging:       "));
  Serial.println (deviceState.isCharging ? "YES" : "no");
  Serial.print    (F("isConnectedToTTN: "));
  Serial.println (deviceState.isConnectedToTTN ? "YES" : "no");
  Serial.print    (F("batteryPercentage: "));
  Serial.print   (deviceState.batteryPercentage);
  Serial.println(F("%"));
  Serial.print    (F("batteryTimeLeft:   "));
  Serial.print   (deviceState.batteryTimeLeft);
  Serial.println(F(" s"));
  Serial.print    (F("mode:              "));
  Serial.println(modeToString(deviceState.mode));
  Serial.print    (F("lightMode:         "));
  Serial.println(lightToString(deviceState.lightMode));
  Serial.println(F("----------------------"));
}

void read_sensors() {
  // Sensor stuff here
}

void print_info_interval() {
  unsigned long now = millis();
  if (now - lastPrintTime >= printInterval) {
    lastPrintTime = now;
    printDeviceState();
    delay(1);
  }
}

void lora_ping() {
  unsigned long now = millis();
  if (now - lastPingTime >= printIntervalLoRa) {
    lastPingTime = now;
    Serial.println("------ LoRa ping routine: ------");

    if (!is_joined_TTN()) { // Test if we are joined
      Serial.println("Not connected - trying to connect...");
      if (join_TTN()) { // ...  if not then do it
      Serial.println("TTN join successful");
      } else {
        Serial.println("TTN join failed");
      }
    }
    // We are online and can do our routine
    Serial.println("Sending status update...");
    TX_RETURN_TYPE response = tranceive(MODULE_NONE, STATUS_OK, "!"); // Send a status update/ping message/I'm alive message
    
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
        processLoRaWANMessage(g_decomposedMessage);
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
    
        
  }
}

void tick_stuff() {
  button1.tick();
  button2.tick();
}

void handleClick1() {
  Serial.println("Btn1 clicked");
}

void handleLong1() {
  Serial.println("Btn1 long press");
  Serial.println("\tGoing into park mode. (do routine)");
  deviceState.mode = PARK;
  goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
}

void handleClick2() {
  Serial.println("Btn2 clicked");
}

void handleLong2() {
  Serial.println("Btn2 long press");
  Serial.println("\tGoing into storage mode. (do routine)");
  deviceState.mode = STORAGE;
  goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
}

void initialize_physical_buttons() {
  // Init OneButton with pull-down support
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  pinMode(BUTTON_2, INPUT_PULLDOWN);

  button1.attachClick(handleClick1);
  button1.attachLongPressStop(handleLong1);

  button2.attachClick(handleClick2);
  button2.attachLongPressStop(handleLong2);
}

void goToDeepSleep(int sleepingTime, bool mpu_interrupt) {
  if (sleepingTime > 0) {
    esp_sleep_enable_timer_wakeup(sleepingTime * uS_TO_S_FACTOR);
  } else {
    Serial.println("(Timer NOT set as wakeup source - only wake on buttons!)");
  }

  if (mpu_interrupt) {
    esp_sleep_enable_ext0_wakeup((gpio_num_t)ADXL345_INT1, HIGH);
    rtc_gpio_pullup_dis((gpio_num_t)ADXL345_INT1);
    rtc_gpio_pulldown_en((gpio_num_t)ADXL345_INT1);
  }

  uint64_t btnMask = (1ULL << BUTTON_1) | (1ULL << BUTTON_2);
  esp_sleep_enable_ext1_wakeup(btnMask, ESP_EXT1_WAKEUP_ANY_HIGH);
  rtc_gpio_pulldown_en((gpio_num_t)BUTTON_1);
  rtc_gpio_pullup_dis((gpio_num_t)BUTTON_1);
  rtc_gpio_pulldown_en((gpio_num_t)BUTTON_2);
  rtc_gpio_pullup_dis((gpio_num_t)BUTTON_2);

  Serial.println("Going to deep sleep...");
  esp_deep_sleep_start();
}

void firstStartupRoutine() {
  Serial.println("\tSince we start in storage mode we will now go to sleep (push buttons to wake up!)");
  goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
}

void mpuWakeupRoutine() {
  Serial.println("\tWe assume that we are always in park mode if we are woken from MPU");
  Serial.println("\tChecking if we are still moving...");
  delay(500); // Simulating continuous MPU measurements
  if (!true) { // Replace with actual movement check
    goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
  }
}

void timerWakeupRoutine() {
  Serial.println("\tWe now have to check which mode we are in and do the right sequence of things!");
  if (deviceState.mode == STORAGE) {
    timerWakeupRoutineFromStorage();
  } else if (deviceState.mode == PARK) {
    timerWakeupRoutineFromPark();
  } else {
    Serial.println("We were in neither park nor storage mode - something is wrong!");
    goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
  }
}

void timerWakeupRoutineFromStorage() {
  Serial.println("Doing storage mode wakeup routine");
  goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
}

void timerWakeupRoutineFromPark() {
  Serial.println("Doing park mode wakeup routine");
  // Write code here
  // Init. stuff
  // send ping...
  Serial.println("EXAMPLE: Scanning for WiFi MACs");

  initWiFiScanner();
  getTop3Networks(lastMacs, lastRssis);
  stopWiFiScanner();

  goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
}
