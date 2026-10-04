#include "SoundSensor.h"
#include "config.h"
#include <Arduino.h>

SoundSensor::SoundSensor(int sensorPin)
{
    pin = sensorPin;
}

void SoundSensor::begin()
{
    pinMode(pin, INPUT);
}

int SoundSensor::readRaw()
{
    int minimum = 4095;
    int maximum = 0;

    for (int i = 0; i < SOUND_SAMPLE_COUNT; i++)
    {
        int reading = analogRead(pin);

        if (reading < minimum)
        {
            minimum = reading;
        }

        if (reading > maximum)
        {
            maximum = reading;
        }

        delayMicroseconds(SOUND_SAMPLE_INTERVAL_US);
    }

    return maximum - minimum;
}