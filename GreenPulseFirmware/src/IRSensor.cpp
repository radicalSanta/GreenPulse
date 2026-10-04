#include "IRSensor.h"
#include "config.h"
#include <Arduino.h>

IRSensor::IRSensor(int sensorPin)
{
    pin = sensorPin;
}

void IRSensor::begin()
{
    pinMode(pin, INPUT);
}

bool IRSensor::isActive()
{
    return digitalRead(pin) == IR_ACTIVE_STATE;
}

float IRSensor::readActivity()
{
    if (isActive())
    {
        return 100.0;
    }

    return 0.0;
}