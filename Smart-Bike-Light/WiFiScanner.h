//file=WiFiScanner.h

#ifndef WIFISCANNER_H
#define WIFISCANNER_H

#include <Arduino.h>   // ← pull in the core + event types
#include <WiFi.h>
#include <string.h> 

void initWiFiScanner();
void stopWiFiScanner();
int getTop3Networks(uint8_t macs[3][6], uint8_t rssis[3]);

#endif
