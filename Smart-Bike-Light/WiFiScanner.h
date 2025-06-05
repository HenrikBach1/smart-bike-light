//file=WiFiScanner.h

#ifndef WIFISCANNER_H
#define WIFISCANNER_H

#include <Arduino.h>   
#include "Globals.h"   

#if ENABLE_WIFI_SCANNER

#include <WiFi.h>
#include <string.h> 

void initWiFiScanner();
void stopWiFiScanner();
int getTop3Networks(uint8_t macs[3][6], uint8_t rssis[3]);

#else

// Empty function stubs when module is disabled
inline void initWiFiScanner() {}
inline void stopWiFiScanner() {}
inline int getTop3Networks(uint8_t macs[3][6], uint8_t rssis[3]) { return 0; }

#endif // ENABLE_WIFI_SCANNER

#endif // WIFISCANNER_H
