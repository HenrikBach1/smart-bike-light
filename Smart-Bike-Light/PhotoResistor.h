#ifndef PHOTORESISTOR_H
#define PHOTORESISTOR_H

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

#endif
