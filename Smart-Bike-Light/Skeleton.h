// Skeleton.h
#ifndef SKELETON_H
#define SKELETON_H

#include <Arduino.h>
#include <OneButton.h>
#include <driver/rtc_io.h>

#include "esp_sleep.h"
#include "WiFiScanner.h"
#include "Globals.h"
#include "CustomLoRa.h"


extern unsigned long lastPrintTime;
extern unsigned long lastPingTime;
extern const unsigned long printInterval;  // milliseconds
extern const unsigned long printIntervalLoRa;  // milliseconds
extern RTC_DATA_ATTR Mode mode;

extern OneButton button1; /* active-HIGH */
extern OneButton button2;

extern uint8_t lastMacs[3][6];
extern uint8_t lastRssis[3];

extern DeviceState deviceState;


// Function prototypes
const char* modeToString(Mode m);
const char* lightToString(LightMode l);
void printDeviceState();
void read_sensors();
void print_info_interval();
void lora_ping();
void tick_stuff();

void handleClick1();
void handleLong1();
void handleClick2();
void handleLong2();

void initialize_physical_buttons();

void goToDeepSleep(int sleepingTime, bool mpu_interrupt);

void firstStartupRoutine();
void mpuWakeupRoutine();
void timerWakeupRoutine();
void timerWakeupRoutineFromStorage();
void timerWakeupRoutineFromPark();

#endif // SKELETON_H
