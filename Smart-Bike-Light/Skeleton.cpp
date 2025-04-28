// Skeleton.cpp
#include "Skeleton.h"

/* This module's compilation is controlled by the ENABLE_SKELETON flag in Globals.h
 * When ENABLE_SKELETON is set to 0, this implementation code is excluded from compilation
 * When ENABLE_SKELETON is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in Skeleton.h when this module is disabled
 */

#if ENABLE_SKELETON // Only compile implementation if module is enabled

unsigned long lastPrintTime = 0;
const unsigned long printInterval = 5000;  // milliseconds
RTC_DATA_ATTR Mode mode = STORAGE;

OneButton button1(BUTTON_1, false); /* active-HIGH */
OneButton button2(BUTTON_2, false);

uint8_t lastMacs[3][6] = {{0x00}};
uint8_t lastRssis[3] = {0x00};


void read_sensors() {
  // Sensor stuff here
}

void print_info_interval() {
  unsigned long now = millis();
  if (now - lastPrintTime >= printInterval) {
    lastPrintTime = now;
    Serial.println("This code executes in a loop in active mode!");
    delay(1);
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
  mode = PARK;
  goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
}

void handleClick2() {
  Serial.println("Btn2 clicked");
}

void handleLong2() {
  Serial.println("Btn2 long press");
  Serial.println("\tGoing into storage mode. (do routine)");
  mode = STORAGE;
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
  if (mode == STORAGE) {
    timerWakeupRoutineFromStorage();
  } else if (mode == PARK) {
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
  Serial.println("EXAMPLE: Scanning for WiFi MACs");

  initWiFiScanner();
  getTop3Networks(lastMacs, lastRssis);
  stopWiFiScanner();

  goToDeepSleep(TIME_TO_SLEEP_PARKED, true);
}

#endif // ENABLE_SKELETON
