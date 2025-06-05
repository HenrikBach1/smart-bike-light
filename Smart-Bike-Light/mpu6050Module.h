// filepath: /home/sad/projects/arduino/ESP32/smart-bike-light/Smart-Bike-Light/mpu6050Module.h

#ifndef MPU6050_MODULE_H
#define MPU6050_MODULE_H

#define HARD_BREAK 30
#define SOFT_BREAK 10
#define STILL_RANGE 2

#define X_OFFSET 2
#define Y_OFFSET 1
#define Z_OFFSET 31

#include "Globals.h" // Include for module enable/disable flags
#include "Pins.h"
#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library
#include "Skeleton.h"

#if ENABLE_MPU6050

extern int lastMessage;
extern ADXL345 adxl;

void initAdx();
int checkBrakeType(int x, int y, int z);
void IRAM_ATTR pin1Interrupt();
void clear_in_activity_int();
void ADXL_ISR();


#else

// Empty function stubs when module is disabled


#endif // ENABLE_MPU6050

#endif // MPU6050_MODULE_H