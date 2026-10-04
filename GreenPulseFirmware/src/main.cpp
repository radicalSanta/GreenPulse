#include <Arduino.h>

#include "config.h"

#include "DHTSensor.h"
#include "SoilSensor.h"
#include "SoundSensor.h"
#include "IRSensor.h"
#include "CO2Sensor.h"
#include "GPS.h"

#include "CircularBuffer.h"
#include "DataProcessor.h"
#include "Telemetry.h"
#include "TelemetrySerializer.h"

#include "WiFiManager.h"
#include "MQTTClient.h"


// ============================================================
// SENSOR OBJECTS
// ============================================================

DHTSensor dhtSensor(DHT_PIN);

SoilSensor soilSensor(SOIL_PIN);

SoundSensor soundSensor(SOUND_PIN);

IRSensor irSensor(IR_PIN);

CO2Sensor co2Sensor(CO2_PIN);

GPS gps(GPS_RX_PIN, GPS_TX_PIN);


// ============================================================
// WIFI / MQTT
// ============================================================

WiFiManager wifi;
MQTTClient mqtt;


// ============================================================
// CIRCULAR BUFFERS
// ============================================================

float soilSamples[SOIL_BUFFER_SIZE];

float soundSamples[SOUND_BUFFER_SIZE];

CircularBuffer soilBuffer(
    soilSamples,
    SOIL_BUFFER_SIZE
);

CircularBuffer soundBuffer(
    soundSamples,
    SOUND_BUFFER_SIZE
);


// ============================================================
// TELEMETRY
// ============================================================

Telemetry data;


// ============================================================
// TIMING
// ============================================================

unsigned long lastSensorRead = 0;


// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("       GREENPULSE ESP32");
    Serial.println("================================");

    // --------------------------------------------------------
    // Sensors
    // --------------------------------------------------------

    dhtSensor.begin();

    soilSensor.begin();

    soundSensor.begin();

    irSensor.begin();

    co2Sensor.begin();

    Serial.println("Sensors initialized");


    // --------------------------------------------------------
    // Wi-Fi
    // --------------------------------------------------------

    wifi.begin();


    // --------------------------------------------------------
    // MQTT
    // --------------------------------------------------------

    mqtt.begin();

    if (mqtt.connect())
    {
        Serial.println("MQTT connection successful");
    }
    else
    {
        Serial.println("MQTT connection failed");
    }
}


// ============================================================
// LOOP
// ============================================================

void loop()
{
    // ========================================================
    // GPS
    // ========================================================

    gps.update();


    // ========================================================
    // MQTT
    // ========================================================

    if (mqtt.isConnected())
    {
        mqtt.loop();
    }
    else
    {
        static unsigned long lastReconnectAttempt = 0;

        unsigned long now = millis();

        if (now - lastReconnectAttempt >= 5000)
        {
            lastReconnectAttempt = now;

            Serial.println("MQTT disconnected - reconnecting...");

            mqtt.connect();
        }
    }


    // ========================================================
    // SENSOR TIMING
    // ========================================================

    unsigned long currentTime = millis();

    if (currentTime - lastSensorRead < SENSOR_INTERVAL_MS)
    {
        return;
    }

    lastSensorRead = currentTime;


    // ========================================================
    // TEMPERATURE + HUMIDITY
    // ========================================================

    float temperature =
        dhtSensor.readTemperature();

    float humidity =
        dhtSensor.readHumidity();

    data.temperature_c = temperature;

    data.humidity_percent = humidity;

    data.temperature_score =
        DataProcessor::calculateTemperatureScore(
            temperature
        );

    data.humidity_score =
        DataProcessor::calculateHumidityScore(
            humidity
        );


    // ========================================================
    // SOIL
    // ========================================================

    int soilRaw =
        soilSensor.readRaw();

    soilBuffer.add(soilRaw);

    float soilAverageRaw =
        soilBuffer.average();

    data.soil_moisture =
        DataProcessor::normalizeSoil(
            soilAverageRaw
        );

    data.soil_score =
        DataProcessor::calculateSoilScore(
            data.soil_moisture
        );


    // ========================================================
    // SOUND
    // ========================================================

    int soundRaw =
        soundSensor.readRaw();

    soundBuffer.add(soundRaw);

    float soundAverageRaw =
        soundBuffer.average();

    data.sound_activity =
        DataProcessor::normalizeSound(
            soundAverageRaw
        );

    data.sound_score =
        DataProcessor::calculateSoundScore(
            data.sound_activity
        );


    // ========================================================
    // IR SENSOR
    // ========================================================
    //
    // Temporarily stored in PIR fields because the backend
    // currently expects pir_activity and pir_score.
    //
    // Later:
    // pir_activity -> ir_activity
    // pir_score    -> ir_score
    //
    // ========================================================

    float irActivity =
        irSensor.readActivity();

    data.pir_activity =
        irActivity;

    data.pir_score =
        DataProcessor::calculateIRScore(
            irActivity
        );


    // ========================================================
    // MQ-135 CO2
    // ========================================================

    int co2Raw =
        co2Sensor.readRaw();

    data.co2_ppm =
        DataProcessor::normalizeCO2(
            co2Raw
        );

    data.co2_score =
        DataProcessor::calculateCO2Score(
            data.co2_ppm
        );


    // ========================================================
    // GPS
    // ========================================================

    data.latitude =
        gps.getLatitude();

    data.longitude =
        gps.getLongitude();

    data.gps_source =
        gps.getSource();


    // ========================================================
    // SERIAL DEBUG
    // ========================================================

    Serial.println();
    Serial.println("----------- GREENPULSE -----------");

    Serial.print("Temperature: ");
    Serial.print(data.temperature_c);
    Serial.print(" C | Score: ");
    Serial.println(data.temperature_score);

    Serial.print("Humidity: ");
    Serial.print(data.humidity_percent);
    Serial.print(" % | Score: ");
    Serial.println(data.humidity_score);

    Serial.print("Soil Moisture: ");
    Serial.print(data.soil_moisture);
    Serial.print(" % | Score: ");
    Serial.println(data.soil_score);

    Serial.print("Sound Activity: ");
    Serial.print(data.sound_activity);
    Serial.print(" | Score: ");
    Serial.println(data.sound_score);

    Serial.print("IR Activity: ");
    Serial.print(data.pir_activity);
    Serial.print(" | Score: ");
    Serial.println(data.pir_score);

    Serial.print("MQ-135 CO2: ");
    Serial.print(data.co2_ppm);
    Serial.print(" ppm | Score: ");
    Serial.println(data.co2_score);

    Serial.print("Latitude: ");
    Serial.println(data.latitude, 6);

    Serial.print("Longitude: ");
    Serial.println(data.longitude, 6);

    Serial.print("GPS Source: ");
    Serial.println(data.gps_source);


    // ========================================================
    // JSON
    // ========================================================

    String json =
        TelemetrySerializer::toJSON(data);

    Serial.println();
    Serial.println("JSON:");
    Serial.println(json);


    // ========================================================
    // MQTT
    // ========================================================

    if (mqtt.isConnected())
    {
        mqtt.publish(json.c_str());

        Serial.println("MQTT: Telemetry published");
    }
    else
    {
        Serial.println(
            "MQTT: Not connected - telemetry not published"
        );
    }

    Serial.println("----------------------------------");
}