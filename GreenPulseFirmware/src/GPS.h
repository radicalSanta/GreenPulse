#ifndef GPS_H
#define GPS_H

#include <HardwareSerial.h>

class GPS
{
private:
    HardwareSerial serial;

public:
    GPS(int rxPin, int txPin);

    void update();

    bool isValid();

    float getLatitude();
    float getLongitude();

    const char* getSource();
};

#endif