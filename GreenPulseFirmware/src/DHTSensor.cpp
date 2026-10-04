#include "DHTSensor.h"

DHTSensor::DHTSensor(int pin)
    : dht(pin, DHT11)
{
}

void DHTSensor::begin()
{
    dht.begin();
}

float DHTSensor::readTemperature()
{
    return dht.readTemperature();
}

float DHTSensor::readHumidity()
{
    return dht.readHumidity();
}