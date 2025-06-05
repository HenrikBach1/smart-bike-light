//file=Globals.h

#ifndef GLOBALS_H
#define GLOBALS_H

#include "Pins.h"    // Include Pins.h for LED_PIN
#include <OneButton.h> 


// Module enable/disable flags
// Set to 0 to exclude a module from compilation
#define ENABLE_BLE_MODULE 0        // Enable BLE functionality

#define ENABLE_MPU6050 1           // Disable MPU6050 module by default
#define ENABLE_LED_BRIGHTNESS 1    // Enable LED Brightness Adjustment

#define ENABLE_SKELETON 1          // Disable Skeleton module
#define ENABLE_WIFI_SCANNER 1      // Enable WiFi Scanner
#define ENABLE_LORA_MODULE 1       // Use CustomLoRa implementation
#define ENABLE_LED_CONTROL 1       // Enable LED Control module
#define ENABLE_PHOTO_RESISTOR 1    // Enable Photo Resistor module

typedef enum {
    MODULE_NONE = 0,
    MODULE_LORAWAN,
    MODULE_BATTERY,
    MODULE_WIFI,
    MODULE_THEFT
} Module;

typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1
} Status;

typedef enum {
  STORAGE,
  PARK,
  ACTIVE,
  STOLEN
} Mode;

typedef enum {
  NO_LIGHT,
  ECO,
  MEDIUM,
  STRONG
} LightMode;

struct DeviceState {
  bool isMoving;
  int isBraking;
  bool isDark;
  bool isCharging;
  int batteryPercentage;
  float batteryTimeLeft;
  Mode mode;
  LightMode lightMode;
  bool light_on;
};


#define UART    1   // UART1
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP_PARKED  30    // Wakeup interval in parked mode
#define TIME_TO_SLEEP_STORAGE  60  // Maybe we should not have timer on storage mode
#define TIME_TO_SLEEP_STOLEN 30
#define TIME_NEVER_WAKEUP -1 // Dont wake up again
#define TIME_INACTIVITY 30 // when do we go to park mode automatically


#endif // GLOBALS_H
