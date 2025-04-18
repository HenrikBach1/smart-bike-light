//file=CustomBLE.h

#ifndef CUSTOMBLE_H
#define CUSTOMBLE_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

class CustomBLE {
public:
    CustomBLE(const char* targetServiceUUID);
    void begin();
    void scan();
    bool isUnlocked() const;
    void lock();
    void setRssiThreshold(int threshold);

private:
    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    public:
        AdvertisedDeviceCallbacks(CustomBLE* parent);
        void onResult(BLEAdvertisedDevice advertisedDevice);

    private:
        CustomBLE* parentBLE;
    };

    BLEScan* pBLEScan;
    BLEUUID unlockServiceUUID;
    bool unlocked;
    int rssiThreshold;

    bool matchesUnlockService(BLEAdvertisedDevice& device);
};


#endif