#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

class DataProcessor
{
public:

    // Temperature
    static float calculateTemperatureScore(float temperature);

    // Humidity
    static float calculateHumidityScore(float humidity);

    // Soil
    static float normalizeSoil(float rawValue);
    static float calculateSoilScore(float moisture);

    // Sound
    static float normalizeSound(float rawValue);
    static float calculateSoundScore(float soundActivity);

    // IR / human activity
    static float calculateIRScore(float activity);

    // CO2
    static float normalizeCO2(float rawValue);
    static float calculateCO2Score(float co2ppm);
};

#endif