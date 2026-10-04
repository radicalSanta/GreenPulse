#include "GPS.h"
#include "config.h"
#include <TinyGPSPlus.h>

static TinyGPSPlus gpsParser;

GPS::GPS(int rxPin, int txPin)
    : serial(2)
{
    serial.begin(9600, SERIAL_8N1, rxPin, txPin);
}

void GPS::update()
{
    while (serial.available())
    {
        gpsParser.encode(serial.read());
    }
}

bool GPS::isValid()
{
    return gpsParser.location.isValid();
}

float GPS::getLatitude()
{
    if (isValid())
    {
        return gpsParser.location.lat();
    }

    return FALLBACK_LATITUDE;
}

float GPS::getLongitude()
{
    if (isValid())
    {
        return gpsParser.location.lng();
    }

    return FALLBACK_LONGITUDE;
}

const char* GPS::getSource()
{
    if (isValid())
    {
        return "GPS";
    }

    return "FALLBACK";
}