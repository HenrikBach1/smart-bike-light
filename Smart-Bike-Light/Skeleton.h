// Skeleton.h
#ifndef SKELETON_H
#define SKELETON_H

#include <Arduino.h>
#include <OneButton.h>
#include <driver/rtc_io.h>

#include <cstdio>    // for snprintf on many C++ toolchains
#include "esp_sleep.h"
#include "WiFiScanner.h"
#include "Globals.h"
#include "CustomLoRa.h"
#include "LedControl.h"
#include "mpu6050Module.h"
#include "LedBrightnessAdjust.h"
#include "PhotoResistor.h"
#include "battery_monitoring.h"


extern unsigned long lastPrintTime;
extern unsigned long lastPingTime;
extern unsigned long lastTickTime;
extern const unsigned long printInterval;  // milliseconds
extern const unsigned long printIntervalLoRa;  // milliseconds
extern const unsigned long printIntervalTick;

extern OneButton button1; /* active-HIGH */
extern OneButton button2;

extern uint8_t lastMacs[3][6];
extern uint8_t lastRssis[3];

extern RTC_DATA_ATTR DeviceState deviceState;

extern PhotoResistor photoResistor;
extern battery_monitoring battery;

// Function prototypes
const char* modeToString(Mode m);
const char* lightToString(LightMode l);
void printDeviceState();
void print_info_interval();
void lora_ping_routine();
void tick_stuff();
void tick_stuff_interval();
static LightMode nextLightMode(LightMode m);
static LightMode prevLightMode(LightMode m);
void toggleLightMode(DeviceState* state, bool up);
void handleClick1();
void handleLong1();
void handleLongStart1();
void handleDoubleClick1();
void handleClick2();
void handleLong2();
void handleLongStart2();
void handleDoubleClick2();

void initialize_physical_buttons();

void goToDeepSleep(int sleepingTime, bool mpu_interrupt, bool stolen = false);

void firstStartupRoutine();
void mpuWakeupRoutine();
void timerWakeupRoutine();
void timerWakeupRoutineFromStorage();
void timerWakeupRoutineFromStolen();
void timerWakeupRoutineFromPark();

#endif // SKELETON_H
