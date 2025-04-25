// Skeleton.h
#ifndef SKELETON_H
#define SKELETON_H

#include <Arduino.h>
#include "Globals.h"  // For ENABLE_SKELETON flag

#if ENABLE_SKELETON // Only compile implementation if module is enabled in Globals.h

#include <OneButton.h>
#include <driver/rtc_io.h>
#include "esp_sleep.h"
#include "WiFiScanner.h"


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

#else

// Empty function stubs when module is disabled
inline void read_sensors() {}
inline void print_info_interval() {}
inline void tick_stuff() {}

inline void handleClick1() {}
inline void handleLong1() {}
inline void handleClick2() {}
inline void handleLong2() {}

inline void initialize_physical_buttons() {}

inline void goToDeepSleep(int sleepingTime, bool mpu_interrupt) {}

inline void firstStartupRoutine() {}
inline void mpuWakeupRoutine() {}
inline void timerWakeupRoutine() {}
inline void timerWakeupRoutineFromStorage() {}
inline void timerWakeupRoutineFromPark() {}

#endif // ENABLE_SKELETON

#endif // SKELETON_H
