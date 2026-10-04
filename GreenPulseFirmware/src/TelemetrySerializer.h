#ifndef TELEMETRY_SERIALIZER_H
#define TELEMETRY_SERIALIZER_H

#include<Arduino.h>
#include "Telemetry.h"

class TelemetrySerializer
{
public:
    static String toJSON(const Telemetry& data);
};

#endif