#ifndef CO2_SENSOR_H
#define CO2_SENSOR_H

class CO2Sensor
{
private:
    int pin;

public:
    CO2Sensor(int sensorPin);

    void begin();

    int readRaw();
};

#endif