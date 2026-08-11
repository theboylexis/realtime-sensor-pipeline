# Realtime Sensor Pipeline

![ESP32](https://img.shields.io/badge/ESP32-000000?style=for-the-badge&logo=espressif&logoColor=white)
![Arduino](https://img.shields.io/badge/Arduino-00979D?style=for-the-badge&logo=arduino&logoColor=white)
![MQTT](https://img.shields.io/badge/MQTT-660066?style=for-the-badge&logo=mqtt&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Plotly](https://img.shields.io/badge/Plotly-3F4F75?style=for-the-badge&logo=plotly&logoColor=white)
![Dash](https://img.shields.io/badge/Dash-008DE4?style=for-the-badge&logo=plotly&logoColor=white)

A real-time IoT sensor pipeline built on the ESP32: sensor data is collected, published over MQTT, and visualized on a live Plotly/Dash web dashboard.

**Stack:** ESP32 → MQTT (Mosquitto) → Python (paho-mqtt) → Dash/Plotly

---

## Overview

The system continuously samples temperature, humidity, light, and distance from an ESP32, publishes the readings as a JSON payload over MQTT, and renders them as live-updating graphs on a web dashboard — no page refresh required.

```
ESP32 (sensors) --> MQTT Broker (Mosquitto) --> Python/Dash Dashboard --> Browser
```

## Features

- Multi-sensor data acquisition on a single ESP32 (temperature, humidity, light, distance)
- JSON payloads published over MQTT every 5 seconds
- Live dashboard with four auto-updating Plotly graphs
- All sensors run off the ESP32's native 3.3V rail — no external resistors or level-shifting required

## Hardware

| Component | ESP32 Pin | Notes |
|---|---|---|
| DHT11 (temperature/humidity) | D4 (GPIO4) | Digital |
| LDR module (light) | GPIO34 | Analog, breakout module with on-board pull-down |
| HC-SR04 (ultrasonic distance) | Trig: D5, Echo: D18 | Powered at 3.3V — see note below |

**Design note:** the HC-SR04 is powered directly from the ESP32's 3.3V rail rather than the more common 5V setup. This removes the need for a voltage-divider on the Echo line (normally required to safely step a 5V echo pulse down for a 3.3V-only GPIO), at the cost of reduced maximum range (~1-1.5m instead of the rated ~4m). For a desk-scale demo this is a reasonable trade-off for a simpler, more reliable wiring loom.

## Repository Structure

```
realtime-sensor-pipeline/
├── firmware/     # ESP32 Arduino sketch (sensor reading + MQTT publishing)
├── dashboard/    # Python Dash app (MQTT subscriber + live graphs)
└── README.md
```

## Getting Started

### 1. Firmware (ESP32)

Requirements — install via Arduino IDE Library Manager:
- `PubSubClient` (Nick O'Leary)
- `ArduinoJson` (Benoit Blanchon)
- `DHT sensor library` + `Adafruit Unified Sensor` (Adafruit)

Open `firmware/*.ino` in Arduino IDE, update the following before flashing:

```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "YOUR_BROKER_IP";
```

Select board **ESP32 Dev Module**, choose the correct COM port, and upload.

### 2. MQTT Broker (Mosquitto)

Install [Mosquitto](https://mosquitto.org/download/) and run it with a config that listens on all interfaces:

```
listener 1883 0.0.0.0
allow_anonymous true
```

```bash
mosquitto -c mosquitto.conf -v
```

Make sure port 1883 is allowed through your firewall if the ESP32/dashboard are on a different machine than the broker.

### 3. Dashboard

```bash
pip install dash plotly paho-mqtt
python dashboard/realtime_dashboard.py
```

Update `MQTT_BROKER` and `MQTT_TOPIC` in the script to match your setup, then open:

```
http://127.0.0.1:8050
```
