#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

// ------------------ Sensor Pins ------------------
#define DHTPIN 4
#define DHTTYPE DHT11
#define LDR_PIN 25
#define TRIG_PIN 5
#define ECHO_PIN 18
#define IR_PIN 15

DHT dht(DHTPIN, DHTTYPE);

// ------------------ WiFi Credentials ------------------
const char* ssid = "Brian";
const char* password = "11111114";

// ------------------ MQTT Broker ------------------
const char* mqtt_server = "172.20.10.3"; // Broker PC's IP

WiFiClient espClient;
PubSubClient client(espClient);

// ------------------ WiFi Setup ------------------
void setup_wifi() {
  Serial.print("Connecting to WiFi");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());
}

// ------------------ MQTT Reconnect ------------------
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker...");

    if (client.connect("ESP32Client_BlackBox33")) {
      Serial.println("Connected!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 2 seconds...");
      delay(2000);
    }
  }
}

// ------------------ Ultrasonic Function ------------------
float readDistanceCM() {

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
    return -1;

  return duration * 0.0343 / 2;
}

// ------------------ Setup ------------------
void setup() {

  Serial.begin(115200);

  dht.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT);

  setup_wifi();

  client.setServer(mqtt_server, 1883);
}

// ------------------ Main Loop ------------------
void loop() {

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  // Read sensors
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int light = analogRead(LDR_PIN);
  float distance = readDistanceCM();

  // LOW = Obstacle detected
  bool obstacleDetected = (digitalRead(IR_PIN) == LOW);

  // Check DHT readings
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT11 read failed.");
    delay(5000);
    return;
  }

  // Create JSON
  StaticJsonDocument<256> doc;

  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["light"] = light;
  doc["distance"] = distance;
  doc["obstacle"] = obstacleDetected;

  char buffer[256];
  serializeJson(doc, buffer);

  // Publish MQTT
  client.publish("esp32/blackbox33/data", buffer);

  // Print to Serial Monitor
  Serial.println(buffer);

  delay(5000);
}