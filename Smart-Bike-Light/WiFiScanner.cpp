//file=WiFiScanner.cpp

#include "WiFiScanner.h"

/* This module's compilation is controlled by the ENABLE_WIFI_SCANNER flag in Globals.h
 * When ENABLE_WIFI_SCANNER is set to 0, this implementation code is excluded from compilation
 * When ENABLE_WIFI_SCANNER is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in WiFiScanner.h when this module is disabled
 */

#if ENABLE_WIFI_SCANNER

void initWiFiScanner() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void stopWiFiScanner() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
}
// Scans and finds up to the 3 networks with the highest RSSI.
//   macs  – an array [3][6] that will be filled with the BSSID bytes
//   rssis – an array [3]   that will be filled with the absolute RSSI (0…255)
// Returns the number of entries written (0–3).
int getTop3Networks(uint8_t macs[3][6], uint8_t rssis[3]) {
  int n = WiFi.scanNetworks();
  if (n <= 0) {
    WiFi.scanDelete();
    return 0;
  }

  // here we track top‑3 indexes and their RSSI
  int topIdx[3]  = { -1, -1, -1 };
  int topRssi[3] = { -10000, -10000, -10000 };

  for (int i = 0; i < n; i++) {
    int r = WiFi.RSSI(i);
    if (r > topRssi[0]) {
      topRssi[2] = topRssi[1];  topIdx[2] = topIdx[1];
      topRssi[1] = topRssi[0];  topIdx[1] = topIdx[0];
      topRssi[0] = r;           topIdx[0] = i;
    }
    else if (r > topRssi[1]) {
      topRssi[2] = topRssi[1];  topIdx[2] = topIdx[1];
      topRssi[1] = r;           topIdx[1] = i;
    }
    else if (r > topRssi[2]) {
      topRssi[2] = r;           topIdx[2] = i;
    }
  }

  int count = min(n, 3);
  for (int j = 0; j < count; j++) {
    // copy MAC
    const uint8_t* b = WiFi.BSSID(topIdx[j]);
    memcpy(macs[j], b, 6);

    // convert to unsigned magnitude
    int mag = topRssi[j] < 0 ? -topRssi[j] : topRssi[j];
    if (mag > 255) mag = 255;
    rssis[j] = (uint8_t)mag;
  }

  WiFi.scanDelete();
  return count;
}

#endif // ENABLE_WIFI_SCANNER
