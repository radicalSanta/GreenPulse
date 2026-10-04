#ifndef IR_SENSOR_H
#define IR_SENSOR_H

class IRSensor
{
private:
    int pin;

public:
    IRSensor(int sensorPin);

    void begin();

    bool isActive();

    float readActivity();
};

#endif