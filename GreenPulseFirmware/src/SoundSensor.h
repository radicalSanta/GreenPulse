#ifndef SOUND_SENSOR_H
#define SOUND_SENSOR_H

class SoundSensor
{
private:
    int pin;

public:
    SoundSensor(int sensorPin);

    void begin();

    int readRaw();
};

#endif