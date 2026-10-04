#include "SoilSensor.h"
#include <Arduino.h>

SoilSensor::SoilSensor(int sensorPin)
{
    pin = sensorPin;
}

void SoilSensor::begin()
{
    pinMode(pin, INPUT);
}

int SoilSensor::readRaw()
{
    return analogRead(pin);
}