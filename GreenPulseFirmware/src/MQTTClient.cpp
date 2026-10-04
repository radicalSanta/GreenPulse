#include "MQTTClient.h"
#include "config.h"
#include <Arduino.h>

MQTTClient::MQTTClient()
    : mqtt(wifiClient)
{
}

void MQTTClient::begin()
{
    mqtt.setServer(MQTT_BROKER, MQTT_PORT);

    // Telemetry JSON is small, but leave sufficient room.
    mqtt.setBufferSize(1024);

    // Keep the connection alive for 60 seconds.
    mqtt.setKeepAlive(60);
}

bool MQTTClient::connect()
{
    Serial.print("Connecting to MQTT...");

    // Generate a unique client ID from the ESP32 MAC.
    uint64_t chipId = ESP.getEfuseMac();

    char clientId[32];

    snprintf(
        clientId,
        sizeof(clientId),
        "GreenPulse-ESP32-%04X%08X",
        (uint16_t)(chipId >> 32),
        (uint32_t)chipId
    );

    Serial.print(" Client ID: ");
    Serial.println(clientId);

    Serial.print(" Broker: ");
    Serial.print(MQTT_BROKER);
    Serial.print(":");
    Serial.println(MQTT_PORT);

    if (mqtt.connect(clientId))
    {
        Serial.println("MQTT connected");

        Serial.print("MQTT topic: ");
        Serial.println(MQTT_TOPIC);

        return true;
    }

    Serial.print("MQTT connection failed, state = ");
    Serial.println(mqtt.state());

    return false;
}

bool MQTTClient::isConnected()
{
    return mqtt.connected();
}

void MQTTClient::publish(const char* message)
{
    if (!mqtt.connected())
    {
        Serial.println("MQTT publish skipped: not connected");
        return;
    }

    bool result = mqtt.publish(
        MQTT_TOPIC,
        message
    );

    if (result)
    {
        Serial.println("MQTT publish successful");
    }
    else
    {
        Serial.println("MQTT publish FAILED");
    }
}

void MQTTClient::loop()
{
    if (mqtt.connected())
    {
        mqtt.loop();
    }
}