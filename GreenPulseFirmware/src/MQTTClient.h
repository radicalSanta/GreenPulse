#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>
#include <WiFiClient.h>

class MQTTClient
{
private:
    WiFiClient wifiClient;
    PubSubClient mqtt;

public:
    MQTTClient();

    void begin();

    bool connect();

    bool isConnected();

    void publish(const char* message);

    void loop();
};

#endif