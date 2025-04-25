// Skeleton.h
#ifndef SKELETON_H
#define SKELETON_H

#include <Arduino.h>
#include <OneButton.h>
#include <driver/rtc_io.h>

#include "esp_sleep.h"
#include "WiFiScanner.h"
#include "Globals.h"


extern unsigned long lastPrintTime;
extern const unsigned long printInterval;  // milliseconds
extern RTC_DATA_ATTR Mode mode;

extern OneButton button1; /* active-HIGH */
extern OneButton button2;

extern uint8_t lastMacs[3][6];
extern uint8_t lastRssis[3];


// Function prototypes
void read_sensors();
void print_info_interval();
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
