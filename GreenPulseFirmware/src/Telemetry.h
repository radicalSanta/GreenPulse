#ifndef TELEMETRY_H
#define TELEMETRY_H

struct Telemetry
{
    // ========================================================
    // Temperature
    // ========================================================

    float temperature_c;
    float temperature_score;

    // ========================================================
    // Humidity
    // ========================================================

    float humidity_percent;
    float humidity_score;

    // ========================================================
    // Soil
    // ========================================================

    float soil_moisture;
    float soil_score;

    // ========================================================
    // Sound
    // ========================================================

    float sound_activity;
    float sound_score;

    // ========================================================
    // IR SENSOR
    //
    // Temporarily using PIR field names for backend
    // compatibility.
    // ========================================================

    float pir_activity;
    float pir_score;

    // ========================================================
    // CO2
    // ========================================================

    float co2_ppm;
    float co2_score;

    // ========================================================
    // GPS
    // ========================================================

    float latitude;
    float longitude;
    const char* gps_source;
};

#endif