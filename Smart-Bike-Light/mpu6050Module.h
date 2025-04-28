// filepath: /home/sad/projects/arduino/ESP32/smart-bike-light/Smart-Bike-Light/mpu6050Module.h

#ifndef MPU6050_MODULE_H
#define MPU6050_MODULE_H

#include "Globals.h" // Include for module enable/disable flags

#if ENABLE_MPU6050

// Function declarations for MPU6050 module
void initializeMPU6050();
bool isMPU6050Moving();
void calibrateMPU6050();
void readMPU6050Data();

#else

// Empty function stubs when module is disabled
inline void initializeMPU6050() {}
inline bool isMPU6050Moving() { return false; }
inline void calibrateMPU6050() {}
inline void readMPU6050Data() {}

#endif // ENABLE_MPU6050

#endif // MPU6050_MODULE_H