#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

#include "Globals.h" // Include for module enable/disable flags

#if ENABLE_PHOTO_RESISTOR

class PhotoResistor {
  public:
    PhotoResistor(int photoresistorPin, int threshold);
    void update();
    bool state();
  private:
    int _photoresistorPin;
    int _state;
    int _threshold;
};

#else

// Empty stub class when module is disabled
class PhotoResistor {
  public:
    PhotoResistor(int, int) {}
    void update() {}
    bool state() { return false; }
};

#endif // ENABLE_PHOTO_RESISTOR

#endif
