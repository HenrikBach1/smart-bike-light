//file=Globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Pins.h"    // Include Pins.h for LED_PIN
#include <OneButton.h> 


// Module enable/disable flags
// Set to 0 to exclude/disable a module from compilation
#define ENABLE_MPU6050 0           // Disable MPU6050 module by default
#define ENABLE_BLE_MODULE 0        // Enable BLE functionality
#define ENABLE_LED_BRIGHTNESS 0    // Enable LED Brightness Adjustment

// Set to 1 to include/enable a module for compilation
#define ENABLE_SKELETON 1          // Disable Skeleton module
#define ENABLE_WIFI_SCANNER 1      // Enable WiFi Scanner
#define ENABLE_LORA_MODULE 1       // Use CustomLoRa implementation
#define ENABLE_LED_CONTROL 1       // Enable LED Control module
#define ENABLE_PHOTO_RESISTOR 1    // Enable Photo Resistor module

typedef enum {
    MODULE_NONE = 0,
    MODULE_LORAWAN
} Module;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_PANIC = -255
} Status;

typedef enum {
  STORAGE,
  PARK,
  ACTIVE
} Mode;

typedef enum {
  ECO,
  MEDIUM,
  STRONG
} LightMode;

struct DeviceState {
  bool isMoving;
  bool isDark;
  bool isCharging;
  bool isConnectedToTTN;
  int batteryPercentage;
  int batteryTimeLeft;
  Mode mode;
  LightMode lightMode;
};


#define UART    1   // UART1
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_PARKED  60    // Wakeup interval in parked mode
#define TIME_TO_SLEEP_STORAGE  -1  // Maybe we should not have timer on storage mode



#endif // GLOBALS_H
