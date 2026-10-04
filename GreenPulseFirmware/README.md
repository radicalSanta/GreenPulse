# NEXUS Firmware

**NEXUS — Northeast Environmental eXtreme-event Understanding System**

ESP32 firmware for collecting environmental telemetry and publishing it over MQTT to a local Mosquitto broker.

## Current Architecture

```text
ESP32 Sensors
     |
     | Wi-Fi
     v
Laptop running Mosquitto
     |
     | MQTT: 1883
     v
nexus/telemetry
     |
     v
FastAPI backend (planned)
```

The current firmware collects:

- Temperature
- Humidity
- Soil moisture
- Soil condition score
- Sound activity
- Sound score
- GPS latitude/longitude
- GPS source (`GPS` or `FALLBACK`)

---

## 1. Hardware

Current hardware:

- ESP32 DevKit
- DHT11
- Capacitive soil moisture sensor with LM393
- LM393 sound sensor
- NEO-6M GPS

### Pin Configuration

| Component | ESP32 Pin |
|---|---:|
| DHT11 DATA | GPIO 4 |
| Soil sensor AO | GPIO 34 |
| Sound sensor AO | GPIO 35 |
| GPS TX → ESP32 RX | GPIO 16 |
| GPS RX ← ESP32 TX | GPIO 17 |

The analog outputs (`AO`) of the soil and sound sensors are used.

---

## 2. PlatformIO Setup

The project uses PlatformIO with the ESP32 Arduino framework.

`platformio.ini` contains:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200

lib_deps =
    adafruit/DHT sensor library
    adafruit/Adafruit Unified Sensor
    mikalhart/TinyGPSPlus
    bblanchon/ArduinoJson
    knolleary/PubSubClient
```

Build:

```bash
pio run
```

Upload:

```bash
pio run --target upload
```

Serial monitor:

```bash
pio device monitor
```

---

## 3. Configure Wi-Fi and MQTT

Edit:

```text
src/config.h
```

Set the Wi-Fi credentials:

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
```

Set the MQTT broker:

```cpp
#define MQTT_BROKER "10.36.232.114"
#define MQTT_PORT 1883
#define MQTT_TOPIC "nexus/telemetry"
```

### Important

The MQTT broker IP must be the **laptop's LAN IP**, not `localhost`.

The development test used:

```text
Laptop: 10.36.232.114
ESP32:  10.36.232.173
Network: 10.36.232.0/24
```

Find the laptop's current IP with:

```bash
ip route
```

or:

```bash
ip addr
```

If the laptop's IP changes, update `MQTT_BROKER` in `src/config.h`.

---

## 4. Install Mosquitto

On Arch Linux / EndeavourOS:

```bash
sudo pacman -S mosquitto
```

Enable and start the service:

```bash
sudo systemctl enable --now mosquitto
```

Check its status:

```bash
systemctl status mosquitto
```

Check that Mosquitto is listening:

```bash
ss -ltnp | grep 1883
```

---

## 5. Configure Mosquitto for ESP32 Access

By default, Mosquitto may listen only on localhost.

That means this works on the laptop:

```text
127.0.0.1:1883
```

but the ESP32 cannot connect to it.

For the local hackathon prototype, Mosquitto was configured to listen on all interfaces.

Edit:

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Add:

```conf
listener 1883 0.0.0.0
allow_anonymous true
```

Then restart Mosquitto:

```bash
sudo systemctl restart mosquitto
```

Verify:

```bash
ss -ltnp | grep 1883
```

### Security Note

`allow_anonymous true` is suitable only for this local prototype/testing setup.

For a deployed system, use MQTT authentication and preferably TLS.

---

## 6. Configure the Firewall

NEXUS uses MQTT TCP port:

```text
1883
```

If `firewalld` is running, check:

```bash
sudo firewall-cmd --state
```

Check the active zone:

```bash
sudo firewall-cmd --get-active-zones
```

For the public zone used during testing, open MQTT permanently:

```bash
sudo firewall-cmd --zone=public --add-port=1883/tcp --permanent
```

Reload:

```bash
sudo firewall-cmd --reload
```

Verify:

```bash
sudo firewall-cmd --zone=public --list-ports
```

You should see:

```text
1883/tcp
```

This is necessary because the ESP32 connects to the laptop over the LAN.

---

## 7. Verify Network Connectivity Before MQTT

Before debugging MQTT, verify that the ESP32 can reach the laptop.

The ESP32 and laptop should be on the same reachable network.

The test setup used:

```text
Laptop IP: 10.36.232.114
ESP32 IP:  10.36.232.173
```

A temporary TCP test was performed on port `5000`.

On the laptop:

```bash
nc -lv 5000
```

The ESP32 connected to the laptop and sent:

```text
Hello from ESP32
```

This confirmed that the basic ESP32 → laptop network path worked.

Port `5000` was only a temporary connectivity test. NEXUS does not use it for MQTT.

---

## 8. MQTT Topic

NEXUS publishes telemetry to:

```text
nexus/telemetry
```

The MQTT broker is:

```text
10.36.232.114:1883
```

Therefore:

```text
ESP32
  |
  | MQTT publish
  v
10.36.232.114:1883
  |
  v
Mosquitto
  |
  v
nexus/telemetry
```

---

## 9. Test MQTT From the Laptop

Install the Mosquitto client utilities if necessary:

```bash
sudo pacman -S mosquitto
```

Subscribe to the NEXUS telemetry topic:

```bash
mosquitto_sub -h 10.36.232.114 -p 1883 -t nexus/telemetry
```

When the ESP32 is running, telemetry should appear continuously.

Example:

```json
{
  "temperature_c": 24.8,
  "humidity_percent": 88,
  "soil_moisture": 0,
  "soil_score": 0,
  "sound_activity": 0.610501,
  "sound_score": 99.3895,
  "latitude": 25.5788,
  "longitude": 91.8933,
  "gps_source": "FALLBACK"
}
```

The JSON may appear wrapped across multiple terminal lines depending on terminal width.

---

## 10. MQTT Troubleshooting

### MQTT connection fails with state `-2`

If the ESP32 reports a PubSubClient connection failure with state `-2`, check:

#### A. Broker IP

Make sure:

```cpp
#define MQTT_BROKER "10.36.232.114"
```

matches the laptop's current LAN IP.

#### B. Mosquitto listener

Check:

```bash
ss -ltnp | grep 1883
```

Make sure Mosquitto is not listening only on:

```text
127.0.0.1:1883
```

The ESP32 needs the laptop's LAN interface.

#### C. Firewall

Check:

```bash
sudo firewall-cmd --zone=public --list-ports
```

Make sure:

```text
1883/tcp
```

is present.

#### D. Same network

Check:

```bash
ip addr
```

The ESP32 must be able to route to the laptop.

---

## 11. Firmware MQTT Implementation

The MQTT functionality is wrapped inside:

```text
src/MQTTClient.h
src/MQTTClient.cpp
```

The class provides:

```cpp
void begin();
bool connect();
bool isConnected();
void publish(const char* message);
void loop();
```

The underlying library is PubSubClient.

The MQTT client is initialized with:

```cpp
mqtt.begin();
```

and connected using:

```cpp
mqtt.connect();
```

The MQTT network loop is maintained with:

```cpp
mqtt.loop();
```

Telemetry is published using:

```cpp
mqtt.publish(json.c_str());
```

---

## 12. Telemetry Processing

The firmware follows this pipeline:

```text
Sensor
  ↓
Raw reading
  ↓
Circular buffer
  ↓
Average
  ↓
Normalization
  ↓
Derived score
  ↓
Telemetry struct
  ↓
JSON
  ↓
MQTT
```

Sensor data is sampled approximately every:

```cpp
#define SENSOR_INTERVAL_MS 2000
```

The soil and sound readings use circular buffers with:

```cpp
#define SOIL_BUFFER_SIZE 10
#define SOUND_BUFFER_SIZE 10
```

This smooths the readings before they are sent.

---

## 13. Soil Calibration

Current calibration values are:

```cpp
#define SOIL_DRY_RAW 4095
#define SOIL_WET_RAW 1500
```

These are provisional values and should be calibrated against the actual sensor.

To inspect the raw value, print:

```cpp
Serial.print("Soil Raw: ");
Serial.println(soilRaw);
```

Record readings for:

1. Dry sensor / dry soil
2. Wet soil

Then update:

```cpp
SOIL_DRY_RAW
SOIL_WET_RAW
```

Do not assume that `4095` and `1500` are universally correct for every sensor.

---

## 14. Sound Sensor

The LM393 sound sensor does not provide calibrated sound pressure level (dB) in the current firmware.

Instead, the firmware measures short-term ADC variation:

```text
maximum ADC - minimum ADC
```

over a sample window.

Current configuration:

```cpp
#define SOUND_SAMPLE_COUNT 100
#define SOUND_SAMPLE_INTERVAL_US 1000
```

This produces a **sound activity** value rather than a dB measurement.

Current normalization:

```cpp
#define SOUND_MIN_RAW 0
#define SOUND_MAX_RAW 300
```

These values are also provisional and should be calibrated using actual quiet, speech, and loud environmental conditions.

---

## 15. GPS

The NEO-6M communicates using a hardware serial interface at:

```text
9600 baud
```

The firmware attempts to obtain a valid GPS location.

If no valid fix is available, it currently uses:

```cpp
#define FALLBACK_LATITUDE 25.5788
#define FALLBACK_LONGITUDE 91.8933
```

The telemetry explicitly reports the source:

```json
"gps_source": "GPS"
```

or:

```json
"gps_source": "FALLBACK"
```

This prevents the fallback coordinates from being mistaken for a real GPS fix.

---

## 16. Current Telemetry Format

The ESP32 currently publishes:

```json
{
  "temperature_c": 24.8,
  "humidity_percent": 88,
  "soil_moisture": 0,
  "soil_score": 0,
  "sound_activity": 0.610501,
  "sound_score": 99.3895,
  "latitude": 25.5788,
  "longitude": 91.8933,
  "gps_source": "FALLBACK"
}
```

The current firmware does **not** calculate ML hazard risk.

ML inference belongs in the backend.

---

## 17. Planned Backend Architecture

The firmware is the first part of the NEXUS system.

The planned local hackathon architecture is:

```text
ESP32
  ↓
Wi-Fi
  ↓
Mosquitto :1883
  ↓
FastAPI :8000
  ↓
┌───────────────┬───────────────┐
│               │               │
ML inference  PostgreSQL    External APIs
│               │               │
└───────────────┴───────────────┘
  ↓
REST API
  ↓
Frontend
```

The ML models will initially run as Python modules inside FastAPI, so **ML does not require a separate network port**.

Planned ports:

| Service | Port |
|---|---:|
| Mosquitto MQTT | `1883` |
| FastAPI | `8000` |
| PostgreSQL | `5432` |
| ML | No separate port |
| Frontend development server | `5173` |

---

## 18. Security

This repository is intended for development/hackathon use.

Before publishing the project:

- Do not commit real Wi-Fi passwords.
- Do not commit API keys.
- Do not expose an anonymous MQTT broker to the public internet.
- Do not expose PostgreSQL directly to the internet.
- Use MQTT authentication for deployment.
- Use HTTPS/TLS for production services.
- Replace fallback GPS coordinates with real GPS data when available.

Keep secrets in local configuration or environment variables rather than committing them to Git.

---

## 19. Quick Start

### Install dependencies

```bash
sudo pacman -S mosquitto
```

### Start Mosquitto

```bash
sudo systemctl enable --now mosquitto
```

### Configure listener

```conf
listener 1883 0.0.0.0
allow_anonymous true
```

### Restart

```bash
sudo systemctl restart mosquitto
```

### Open firewall

```bash
sudo firewall-cmd --zone=public --add-port=1883/tcp --permanent
sudo firewall-cmd --reload
```

### Configure ESP32

Edit:

```text
src/config.h
```

Set:

```cpp
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define MQTT_BROKER "YOUR_LAPTOP_IP"
#define MQTT_PORT 1883
#define MQTT_TOPIC "nexus/telemetry"
```

### Build and upload

```bash
pio run --target upload
```

### Monitor

```bash
pio device monitor
```

### Subscribe to telemetry

On the laptop:

```bash
mosquitto_sub -h YOUR_LAPTOP_IP -p 1883 -t nexus/telemetry
```

If JSON telemetry appears, the complete ESP32 → Wi-Fi → MQTT → Mosquitto pipeline is working.

---

## License

Add the project's chosen license before public release.
