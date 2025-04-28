#include "CustomBLE.h"

/* This module's compilation is controlled by the ENABLE_BLE_MODULE flag in Globals.h
 * When ENABLE_BLE_MODULE is set to 0, this implementation code is excluded from compilation
 * When ENABLE_BLE_MODULE is set to 1, this implementation code is included in compilation
 * Empty function stubs are provided in CustomBLE.h when this module is disabled
 */

#if ENABLE_BLE_MODULE

CustomBLE::CustomBLE(const char* targetServiceUUID)
  : pBLEScan(nullptr), unlockServiceUUID(BLEUUID(targetServiceUUID)), unlocked(false), rssiThreshold(-75) {}

void CustomBLE::begin() {
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks(this));
    pBLEScan->setActiveScan(false);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);
}

void CustomBLE::scan() {
    if (!unlocked) {
        Serial.println("Scanning for devices...");
        pBLEScan->start(3, false); // Scan for 3 seconds (non-blocking)
        pBLEScan->clearResults();
    }
}

bool CustomBLE::isUnlocked() const {
    return unlocked;
}

void CustomBLE::lock() {
    unlocked = false;
}

void CustomBLE::setRssiThreshold(int threshold) {
    rssiThreshold = threshold;
}

// Private helper to check if a device matches the unlock UUID
bool CustomBLE::matchesUnlockService(BLEAdvertisedDevice& device) {
    if (!device.haveServiceUUID()) {
        return false;
    }

    int serviceCount = device.getServiceUUIDCount();
    for (int i = 0; i < serviceCount; i++) {
        BLEUUID serviceUUID = device.getServiceUUID(i);
        if (serviceUUID.equals(unlockServiceUUID)) {
            return true;
        }
    }
    return false;
}

// Inner class to handle scan callbacks
CustomBLE::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(CustomBLE* parent)
  : parentBLE(parent) {}

void CustomBLE::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice) {
    int rssi = advertisedDevice.getRSSI();

    if (rssi > parentBLE->rssiThreshold) { // FIRST: check proximity
        if (parentBLE->matchesUnlockService(advertisedDevice)) { // THEN: check services
            Serial.print("Found close device with matching service UUID! RSSI: ");
            Serial.println(rssi);
            parentBLE->unlocked = true;
        }
    } else {
        // Serial.println("Device too far away, ignoring without checking services.");
    }
}

#endif // ENABLE_BLE_MODULE
