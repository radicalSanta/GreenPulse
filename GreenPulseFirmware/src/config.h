#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// SENSOR PINS
// ============================================================

#define DHT_PIN 4
#define SOIL_PIN 34
#define SOUND_PIN 35

// IR obstacle/activity sensor
#define IR_PIN 27

// MQ-135 analog output
#define CO2_PIN 32

// ============================================================
// GPS PINS
// ============================================================

#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

// ============================================================
// WIFI
// ============================================================

#define WIFI_SSID "My phoneeeeeeee"
#define WIFI_PASSWORD "ApricotSpatula"

// ============================================================
// MQTT
// ============================================================

#define MQTT_BROKER "10.169.12.174"
#define MQTT_PORT 1883
#define MQTT_TOPIC "greenpulse/telemetry"

// ============================================================
// SENSOR TIMING
// ============================================================

#define SENSOR_INTERVAL_MS 2000

// ============================================================
// CIRCULAR BUFFERS
// ============================================================

#define SOIL_BUFFER_SIZE 10
#define SOUND_BUFFER_SIZE 10

// ============================================================
// SOIL CALIBRATION
// ============================================================

#define SOIL_DRY_RAW 4095
#define SOIL_WET_RAW 1500

// ============================================================
// SOUND CALIBRATION
// ============================================================

#define SOUND_MIN_RAW 0
#define SOUND_MAX_RAW 4095

#define SOUND_SAMPLE_COUNT 100
#define SOUND_SAMPLE_INTERVAL_US 1000

// ============================================================
// IR SENSOR
// ============================================================

// Most common IR obstacle modules output LOW when an object
// is detected. Change to HIGH if your module behaves opposite.
#define IR_ACTIVE_STATE LOW

// ============================================================
// MQ-135 CO2 ESTIMATION
// ============================================================

// These are INITIAL calibration values.
// They should later be calibrated against a known CO2 value.
//
// ADC_MIN -> approximately CO2_MIN_PPM
// ADC_MAX -> approximately CO2_MAX_PPM

#define CO2_ADC_MIN 300
#define CO2_ADC_MAX 3000

#define CO2_MIN_PPM 400
#define CO2_MAX_PPM 2000

// ============================================================
// GPS FALLBACK
// ============================================================

#define FALLBACK_LATITUDE 25.5788
#define FALLBACK_LONGITUDE 91.8933

#endif