#include "DataProcessor.h"
#include "config.h"
#include <Arduino.h>
#include <math.h>

// ============================================================
// TEMPERATURE SCORE
// ============================================================
//
// Ideal band for the current GreenPulse model:
// 21 - 23 C = 100
//
// Score decreases as temperature moves away from the ideal band.
//
// 5 C  -> 0
// 21-23 C -> 100
// 30 C -> 0
//
// ============================================================

float DataProcessor::calculateTemperatureScore(float temperature)
{
    if (isnan(temperature))
        return 0.0;

    const float IDEAL_MIN = 21.0;
    const float IDEAL_MAX = 23.0;

    const float LOWER_LIMIT = 5.0;
    const float UPPER_LIMIT = 30.0;

    if (temperature >= IDEAL_MIN &&
        temperature <= IDEAL_MAX)
    {
        return 100.0;
    }

    float score;

    if (temperature < IDEAL_MIN)
    {
        score =
            ((temperature - LOWER_LIMIT) /
            (IDEAL_MIN - LOWER_LIMIT)) * 100.0;
    }
    else
    {
        score =
            ((UPPER_LIMIT - temperature) /
            (UPPER_LIMIT - IDEAL_MAX)) * 100.0;
    }

    return constrain(score, 0.0, 100.0);
}


// ============================================================
// HUMIDITY SCORE
// ============================================================
//
// Ideal humidity band:
// 60 - 80 % = 100
//
// Outside this range the score decreases.
// ============================================================

float DataProcessor::calculateHumidityScore(float humidity)
{
    if (isnan(humidity))
        return 0.0;

    const float IDEAL_MIN = 60.0;
    const float IDEAL_MAX = 80.0;

    const float LOWER_LIMIT = 30.0;
    const float UPPER_LIMIT = 100.0;

    if (humidity >= IDEAL_MIN &&
        humidity <= IDEAL_MAX)
    {
        return 100.0;
    }

    float score;

    if (humidity < IDEAL_MIN)
    {
        score =
            ((humidity - LOWER_LIMIT) /
            (IDEAL_MIN - LOWER_LIMIT)) * 100.0;
    }
    else
    {
        score =
            ((UPPER_LIMIT - humidity) /
            (UPPER_LIMIT - IDEAL_MAX)) * 100.0;
    }

    return constrain(score, 0.0, 100.0);
}


// ============================================================
// SOIL NORMALIZATION
// ============================================================

float DataProcessor::normalizeSoil(float rawValue)
{
    float moisture =
        ((SOIL_DRY_RAW - rawValue) /
        (float)(SOIL_DRY_RAW - SOIL_WET_RAW)) * 100.0;

    return constrain(moisture, 0.0, 100.0);
}


// ============================================================
// SOIL SCORE
// ============================================================
//
// Ideal soil moisture approximately 50%.
// Score decreases with deviation.
// ============================================================

float DataProcessor::calculateSoilScore(float moisture)
{
    float deviation = abs(moisture - 50.0);

    float score =
        100.0 - (deviation * 2.0);

    return constrain(score, 0.0, 100.0);
}


// ============================================================
// SOUND NORMALIZATION
// ============================================================

float DataProcessor::normalizeSound(float rawValue)
{
    float activity =
        ((rawValue - SOUND_MIN_RAW) /
        (float)(SOUND_MAX_RAW - SOUND_MIN_RAW)) * 100.0;

    return constrain(activity, 0.0, 100.0);
}


// ============================================================
// SOUND SCORE
// ============================================================
//
// More sound activity = worse environmental condition.
// ============================================================

float DataProcessor::calculateSoundScore(float soundActivity)
{
    float score =
        100.0 - soundActivity;

    return constrain(score, 0.0, 100.0);
}


// ============================================================
// IR / HUMAN ACTIVITY SCORE
// ============================================================
//
// IR activity:
// 0   = no detected activity
// 100 = detected activity
//
// More activity = lower score.
// ============================================================

float DataProcessor::calculateIRScore(float activity)
{
    float score =
        100.0 - activity;

    return constrain(score, 0.0, 100.0);
}


// ============================================================
// MQ-135 RAW ADC -> APPROXIMATE CO2
// ============================================================
//
// IMPORTANT:
// MQ-135 is not a true calibrated CO2 sensor.
// This is an initial approximation.
//
// Higher ADC -> higher estimated concentration.
//
// ============================================================

float DataProcessor::normalizeCO2(float rawValue)
{
    float ppm =
        CO2_MIN_PPM +
        ((rawValue - CO2_ADC_MIN) /
        (float)(CO2_ADC_MAX - CO2_ADC_MIN)) *
        (CO2_MAX_PPM - CO2_MIN_PPM);

    return constrain(
        ppm,
        CO2_MIN_PPM,
        CO2_MAX_PPM
    );
}


// ============================================================
// CO2 SCORE
// ============================================================
//
// Lower CO2 = better.
// 400 ppm = approximately 100
// 2000 ppm = approximately 0
// ============================================================

float DataProcessor::calculateCO2Score(float co2ppm)
{
    float score =
        ((CO2_MAX_PPM - co2ppm) /
        (float)(CO2_MAX_PPM - CO2_MIN_PPM)) * 100.0;

    return constrain(score, 0.0, 100.0);
}