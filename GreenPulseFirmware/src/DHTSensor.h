#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <DHT.h>

class DHTSensor
{
private:
    DHT dht;

public:
    DHTSensor(int pin);

    void begin();

    float readTemperature();
    float readHumidity();
};

#endif