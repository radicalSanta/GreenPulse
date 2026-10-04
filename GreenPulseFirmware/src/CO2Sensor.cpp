#include "CO2Sensor.h"
#include <Arduino.h>

CO2Sensor::CO2Sensor(int sensorPin)
{
    pin = sensorPin;
}

void CO2Sensor::begin()
{
    pinMode(pin, INPUT);
}

int CO2Sensor::readRaw()
{
    return analogRead(pin);
}