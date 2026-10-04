#include "WiFiManager.h"
#include "config.h"
#include <WiFi.h>

void WiFiManager::begin()
{
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}