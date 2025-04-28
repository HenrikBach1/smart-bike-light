//file=Globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Pins.h"    // Include Pins.h for LED_PIN

// Module enable/disable flags
// Set to 0 to exclude a module from compilation
#define ENABLE_SKELETON 0          // Disable Skeleton module
#define ENABLE_MPU6050 0           // Disable MPU6050 module by default

#define ENABLE_WIFI_SCANNER 1      // Enable WiFi Scanner
#define ENABLE_BLE_MODULE 1        // Enable BLE functionality
#define ENABLE_LORA_MODULE 1       // Use CustomLoRa implementation
#define ENABLE_LED_CONTROL 1       // Enable LED Control module
#define ENABLE_LED_BRIGHTNESS 1    // Enable LED Brightness Adjustment
#define ENABLE_PHOTO_RESISTOR 1    // Enable Photo Resistor module

// Common type definitions needed by multiple modules
typedef enum {
    MODULE_NONE = 0,
    MODULE_LORAWAN
} Module;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1
} Status;

typedef enum {
  STORAGE,
  PARK,
  ACTIVE
} Mode;

#define UART    1   // UART1
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_PARKED  60    // Wakeup interval in parked mode
#define TIME_TO_SLEEP_STORAGE  -1  // Maybe we should not have timer on storage mode


#endif // GLOBALS_H
