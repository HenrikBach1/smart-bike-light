
// Skeleton.cpp
#include "Skeleton.h"


unsigned long lastPrintTime = 0;
unsigned long lastPingTime = 0;
unsigned long lastTickTime = 0;
const unsigned long printInterval = 5000;  // milliseconds
const unsigned long printIntervalLoRa = 30000;  // milliseconds
const unsigned long printIntervalTick = 500;  // milliseconds


OneButton button1(BUTTON_1, false); /* active-HIGH */
OneButton button2(BUTTON_2, false);

uint8_t lastMacs[3][6] = {{0x00}};
uint8_t lastRssis[3] = {0x00};

RTC_DATA_ATTR DeviceState deviceState = {
  .isMoving = true,
  .isBraking = 0,
  .isDark = true,
  .isCharging = false,
  .batteryPercentage = 50,
  .batteryTimeLeft = 0,
  .mode = STORAGE,
  .lightMode = ECO,
  .light_on = false // Start with light off (but if clicked again turn on)
};

PhotoResistor photoResistor(LDR_ADC_INPUT, 750);
battery_monitoring battery;

// helper to turn enums into human-readable strings:
const char* modeToString(Mode m) {
  switch(m) {
    case STORAGE:      return "STORAGE";
    case PARK:  return "PARK";
    case ACTIVE:  return "ACTIVE";
    case STOLEN: return "STOLEN";
    // …add other modes here…
    default:           return "UNKNOWN";
  }
}
const char* lightToString(LightMode l) {
  switch(l) {
    case ECO:    return "ECO";
    case MEDIUM: return "MEDIUM";
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
  Serial.print    (F("isBraking:         "));
  Serial.println (deviceState.isBraking);
  Serial.print    (F("isDark:           "));
  Serial.println (deviceState.isDark   ? "YES" : "no");
  Serial.print    (F("isCharging:       "));
  Serial.println (deviceState.isCharging ? "YES" : "no");
  Serial.print    (F("light_on:       "));
  Serial.println (deviceState.light_on ? "YES" : "no");
  Serial.print    (F("batteryPercentage: "));
  Serial.print   (deviceState.batteryPercentage);
  Serial.println(F("%"));
  Serial.print    (F("batteryTimeLeft:   "));
  Serial.print   (deviceState.batteryTimeLeft);
  Serial.println(F(" hours"));
  Serial.print    (F("mode:              "));
  Serial.println(modeToString(deviceState.mode));
  Serial.print    (F("lightMode:         "));
  Serial.println(lightToString(deviceState.lightMode));
  Serial.println(F("----------------------"));
}



void print_info_interval() {
  unsigned long now = millis();
  if (now - lastPrintTime >= printInterval) {
    lastPrintTime = now;
    printDeviceState();
    delay(1);
  }
}

void lora_ping_routine() {
  unsigned long now = millis();
  if (now - lastPingTime >= printIntervalLoRa) {
    Serial.println("------ LoRa ping routine: ------");

    if (!is_joined_TTN()) { // Test if we are joined
      Serial.println("Not connected - trying to connect...");
      if (join_TTN()) { // ...  if not then do it
      Serial.println("TTN join successful");
      } else {
        Serial.println("TTN join failed");
      }
    }

    if (is_joined_TTN()) { //If the connection was succesful send the message
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
      
    } else {
      Serial.println("No connection, so we skip the status update...");
    }
    
    lastPingTime = now; // UPdate value here

  }
}

void tick_stuff() {
  button1.tick();
  button2.tick();




  if (!deviceState.isMoving) { // If not moving -> go to park mode
    //leave_TTN(); // Leave TTN before sleep
    deviceState.mode = PARK;
    goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
  }


  if (deviceState.isBraking) { // If we are braking adjust the light accordingly
    adjustBrightnessSimple(80 + deviceState.isBraking * 10);
  }

  // -- Check movement

  ADXL_ISR(); //Check for interrupts on the ADXL345


  int x,y,z;
  int rtrnVal;
  adxl.readAccel(&x, &y, &z);         // Read the accelerometer values and store them in variables declared above x,y,z
  rtrnVal = checkBrakeType(x, y, z);

  if (rtrnVal == 0) { // If we return 0 from brake function -> set device state is moving to false
    // deviceState.isMoving = false;
  } else {
    // deviceState.isMoving = true;
  }

  if (rtrnVal > 0) {
    deviceState.isBraking = rtrnVal; // Set isbraking to either 0, 1, or 2
  } else {
    deviceState.isBraking = 0;
    adjustBrightnessSimple(0);
  }
  
    // -- Check brightness outside
  photoResistor.update();
  deviceState.isDark = photoResistor.state();


  if ((deviceState.light_on || deviceState.isDark) && !(deviceState.isBraking > 0)) { // If we are supposed to turn on the light, if we are braking dont control the light here!
    switch (deviceState.lightMode) {
      case ECO:
        // Setting brightness to 15%
        adjustBrightnessSimple(10);
        break;
      case MEDIUM:
        // Setting brightness to 60%
        adjustBrightnessSimple(40);
        break;
      case STRONG:
        // Setting brightness to 100%
        adjustBrightnessSimple(80);
        break;
      default:
        adjustBrightnessSimple(50);
        break;
    }
  }


  // Sensor stuff here
  // eg.

  

  

}

void tick_stuff_interval() {
  unsigned long now = millis();
    if (now - lastTickTime >= printIntervalTick) {
      lastTickTime = now;
      // -- Check battery stuff
      deviceState.batteryPercentage = battery.battery_percentage();
      deviceState.isCharging = battery.charging();
      deviceState.batteryTimeLeft = battery.time_left(deviceState.lightMode);
      
    }

}


static LightMode nextLightMode(LightMode m) {
  return (m == STRONG) ? ECO : static_cast<LightMode>(m + 1);
}

static LightMode prevLightMode(LightMode m) {
  return (m == ECO)    ? STRONG : static_cast<LightMode>(m - 1);
}

/**
 * Toggle the lightMode in state up or down.
 *
 * @param state  pointer to your device state
 * @param up     if true, go to the next mode; otherwise go to the previous
 */
void toggleLightMode(DeviceState* state, bool up) {
  if (up) {
    state->lightMode = nextLightMode(state->lightMode);
  } else {
    state->lightMode = prevLightMode(state->lightMode);
  }
}

void handleClick1() {
  Serial.println("Btn1 clicked");
  Serial.println("Change mode DOWN");
  deviceState.light_on = true;
  toggleLightMode(&deviceState, false);
}

void handleLong1() {
  Serial.println("\tGoing into park mode. (do routine)");
  //leave_TTN(); // Leave TTN before sleep
  deviceState.light_on = false;
  deviceState.mode = PARK;
  goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
}

void handleLongStart1() {
  adjustBrightnessSimple(0);
  deviceState.light_on = false;
  Serial.println("Btn1 long press");
  // Turn of light here!
}

void handleDoubleClick1() {
  Serial.println("Btn1 double click");
  deviceState.light_on = false;
  goToDeepSleep(TIME_NEVER_WAKEUP, false);
}

void handleClick2() {
  Serial.println("Btn2 clicked");
  Serial.println("Change mode UP");
  deviceState.light_on = true;
  toggleLightMode(&deviceState, true);
}

void handleLong2() {
  Serial.println("\tGoing into storage mode. (do routine)");
  //leave_TTN(); // Leave TTN before sleep
  deviceState.light_on = false;
  deviceState.mode = STORAGE;
  goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
}

void handleLongStart2() {
  adjustBrightnessSimple(0);
  deviceState.light_on = false;
  Serial.println("Btn2 long press");
  // Turn of light here!
}

void handleDoubleClick2() {
  Serial.println("Btn2 double click");
  deviceState.light_on = false;
  goToDeepSleep(TIME_NEVER_WAKEUP, false);
}



void initialize_physical_buttons() {
  // Init OneButton with pull-down support
  pinMode(BUTTON_1, INPUT_PULLDOWN);
  pinMode(BUTTON_2, INPUT_PULLDOWN);

  button1.attachClick(handleClick1);
  button1.attachLongPressStop(handleLong1);
  button1.attachLongPressStart(handleLongStart1);
  button1.attachDoubleClick(handleDoubleClick1);

  button2.attachClick(handleClick2);
  button2.attachLongPressStop(handleLong2);
  button2.attachLongPressStart(handleLongStart2);
  button2.attachDoubleClick(handleDoubleClick2);
} 

void goToDeepSleep(int sleepingTime, bool mpu_interrupt, bool stolen) { // This should only go to sleep, nothing else

  //deepSleep(); // Send lora module to sleep , goodnight boys
  clear_in_activity_int(); // Clear MPU int registers

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

  if (!stolen) { // Only enable button wakeup if not stolen
    uint64_t btnMask = (1ULL << BUTTON_1) | (1ULL << BUTTON_2);
    esp_sleep_enable_ext1_wakeup(btnMask, ESP_EXT1_WAKEUP_ANY_HIGH);
    rtc_gpio_pulldown_en((gpio_num_t)BUTTON_1);
    rtc_gpio_pullup_dis((gpio_num_t)BUTTON_1);
    rtc_gpio_pulldown_en((gpio_num_t)BUTTON_2);
    rtc_gpio_pullup_dis((gpio_num_t)BUTTON_2);
  }
  
  if (stolen) {
    Serial.println("STOLEN!");
    shortBlinkLed(5);
  }

  Serial.println("Going to deep sleep...");
  esp_deep_sleep_start();
}

void firstStartupRoutine() { // routine on first startup
  Serial.println("\tSince we start in storage mode we will now go to sleep (push buttons to wake up!)");
  initialize_LoRaWAN();
  goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
}

void mpuWakeupRoutine() { // What should happen when we wake up from the mpu wakeup
  Serial.println("\tWe assume that we are always in park mode if we are woken from MPU");
  Serial.println("\tChecking if we are still moving...");
  // If we setup the MPU interrupt to only wake up on continous movement, this is (maybe) redundant
  deviceState.light_on = true; // Set light to turn on
}

void timerWakeupRoutine() { // Dont change this anymore, we should only change the specific routines
  Serial.println("\tWe now have to check which mode we are in and do the right sequence of things!");
  //wakeUp(); // Wake LoRa up
  initPwrLed();
  initialize_LED();
  initialize_LoRaWAN();
  adjustBrightnessSimple(0); // Turn LED completely off
  if (deviceState.mode == STORAGE) {
    timerWakeupRoutineFromStorage();
  } else if (deviceState.mode == PARK) {
    timerWakeupRoutineFromPark();
  } else if (deviceState.mode == STOLEN) {
    timerWakeupRoutineFromStolen();
  } else {
    Serial.println("We were in neither park nor storage mode - something is wrong!");
    goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
  }
}

void timerWakeupRoutineFromStolen() {
  Serial.println("Doing STOLEN mode wakeup routine");
  if (!is_joined_TTN()) { // Test if we are joined
      Serial.println("Not connected - trying to connect...");
      led_on();
      if (join_TTN()) { // ...  if not then do it
      Serial.println("TTN join successful");
      } else {
        Serial.println("TTN join failed");
      }
      led_off();
    } else {
      Serial.println("We assume connecting to TTN is established");
    }

  if (is_joined_TTN()) { // If we joined --> scan for wifi and send
    Serial.println("Trying to send mac adresses via LoRa...");
    initWiFiScanner();

    DecomposedMessage dummyWiFiCommand = { // These are to trick the below function to do WiFi scan and send
      .toModule = MODULE_WIFI, 
      .data = "2"
    };

    processLoRaWANMessage(dummyWiFiCommand); // Send Mac dresses

    stopWiFiScanner();
    //leave_TTN(); // Leave TTN before sleep
  } else {
    Serial.println("Not connected - so not doing wifi scan");
  }

  // Here we should do a ping, receive message to check for mode switch, and switch if neccesary.
  if ((g_decomposedMessage.toModule == MODULE_THEFT)) {
    if (g_decomposedMessage.data == "0") {
      deviceState.mode = STORAGE;
      goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
    } else if (g_decomposedMessage.data == "1")
      deviceState.mode = STOLEN;
      goToDeepSleep(TIME_TO_SLEEP_STOLEN, false, true);
  } else {
    deviceState.mode = STOLEN;
    goToDeepSleep(TIME_TO_SLEEP_STOLEN, false, true);
  }

}

void timerWakeupRoutineFromStorage() {
  Serial.println("Doing storage mode wakeup routine");
  // Send battery percentage here (nothing else)
  // deviceState.batteryPercentage = battery.adc_voltage_to_bb_percent();
  if (!is_joined_TTN()) { // Test if we are joined
      Serial.println("Not connected - trying to connect...");
      if (join_TTN()) { // ...  if not then do it
      Serial.println("TTN join successful");
      } else {
        Serial.println("TTN join failed");
      }
    }

    if (is_joined_TTN()) { //If the connection was succesful send the message
      // We are online and can do our routine
      Serial.println("Sending battery percentage...");
      // 1) Convert to C‐string in a 4-byte buffer:
      char dataBuf[4];  
      snprintf(dataBuf, sizeof(dataBuf), "%u", (unsigned)deviceState.batteryPercentage);
      TX_RETURN_TYPE response = tranceive(MODULE_BATTERY, STATUS_OK, dataBuf); // Send a status update/ping message/I'm alive message
      
      // Display response status
      if (response == TX_SUCCESS) {
        Serial.println("Sent battery percentage successfully");
      } else {
        Serial.println("Failed to send battery percentage");
      }
    } else {
      Serial.println("Connection unsuccesful: Skipping message");
    }
  // We did what we wanted to do, now go to sleep
  //leave_TTN(); // Leave TTN before sleep
  if ((g_decomposedMessage.toModule == MODULE_THEFT) && g_decomposedMessage.data == "1") {
    deviceState.mode = STOLEN;
    goToDeepSleep(TIME_TO_SLEEP_STOLEN, false, true);
  } else {
    deviceState.mode = STORAGE;
    goToDeepSleep(TIME_TO_SLEEP_STORAGE, false);
  }

}

void timerWakeupRoutineFromPark() {
  Serial.println("Doing park mode wakeup routine");
  // Write code here
  // Init. stuff
  // send ping...

  if (!is_joined_TTN()) { // Test if we are joined
      Serial.println("Not connected - trying to connect...");
      led_on();
      if (join_TTN()) { // ...  if not then do it
      Serial.println("TTN join successful");
      } else {
        Serial.println("TTN join failed");
      }
      led_off();
    } else {
      Serial.println("We assume connecting to TTN is established");
    }

  if (is_joined_TTN()) { // If we joined --> scan for wifi and send
    Serial.println("Trying to send mac adresses via LoRa...");
    initWiFiScanner();

    DecomposedMessage dummyWiFiCommand = { // These are to trick the below function to do WiFi scan and send
      .toModule = MODULE_LORAWAN, 
      .data = "2"
    };

    processLoRaWANMessage(dummyWiFiCommand); // Send Mac dresses

    stopWiFiScanner();
    //leave_TTN(); // Leave TTN before sleep
  } else {
    Serial.println("Not connected - so not doing wifi scan");
  }

  // Here we should do a ping, receive message to check for mode switch, and switch if neccesary.

  if ((g_decomposedMessage.toModule == MODULE_THEFT) && g_decomposedMessage.data == "1") {
      deviceState.mode = STOLEN;
      goToDeepSleep(TIME_TO_SLEEP_STOLEN, false, true);
    } else {
      deviceState.mode = PARK;
      goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
    }}
