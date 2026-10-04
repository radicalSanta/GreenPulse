#include "TelemetrySerializer.h"
#include <ArduinoJson.h>

String TelemetrySerializer::toJSON(const Telemetry& data)
{
    JsonDocument doc;

    // Temperature
    doc["temperature_c"] = data.temperature_c;
    doc["temperature_score"] = data.temperature_score;

    // Humidity
    doc["humidity_percent"] = data.humidity_percent;
    doc["humidity_score"] = data.humidity_score;

    // Soil
    doc["soil_moisture"] = data.soil_moisture;
    doc["soil_score"] = data.soil_score;

    // Sound
    doc["sound_activity"] = data.sound_activity;
    doc["sound_score"] = data.sound_score;

    // IR -> temporary PIR-compatible fields
    doc["pir_activity"] = data.pir_activity;
    doc["pir_score"] = data.pir_score;

    // CO2
    doc["co2_ppm"] = data.co2_ppm;
    doc["co2_score"] = data.co2_score;

    // GPS
    doc["latitude"] = data.latitude;
    doc["longitude"] = data.longitude;
    doc["gps_source"] = data.gps_source;

    String output;

    serializeJson(doc, output);

    return output;
}