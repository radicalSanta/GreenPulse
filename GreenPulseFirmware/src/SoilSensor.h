#ifndef SOIL_SENSOR_H
#define SOIL_SENSOR_H

class SoilSensor
{
private:
    int pin;

public:
    SoilSensor(int sensorPin);

    void begin();

    int readRaw();
};

#endif