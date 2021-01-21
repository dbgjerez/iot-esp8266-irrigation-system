#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <AsyncMqttClient.h>

#define FC28_PIN A0
#define DELAY 5000
#define WIFI_SSID ""
#define WIFI_PASS ""
#define MQTT_HOST IPAddress(192, 168, 1, 10)
#define MQTT_PORT 1883

static const int MIN_RANGE = 0;
static const int MAX_RANGE = 1023;
static const int CEN = 100;

AsyncMqttClient mqttClient;

void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

}

void onMqttPublish(uint16_t packetId) {
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al wifi...");
  }

  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());

  Serial.println("0");
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  Serial.println("1");
  mqttClient.connect();
  mqttClient.publish("test/one", 0, false, "a");
  Serial.println("2");
}

void loop() {
  int humidity = analogRead(FC28_PIN);
  float humidityPercent = CEN - map(humidity, MIN_RANGE, MAX_RANGE, MIN_RANGE, CEN);
  Serial.print("Sensor: ");
  Serial.print(humidity);
  Serial.print(" - ");
  Serial.print(humidityPercent);
  Serial.println("%");
  if(mqttClient.connected()){
    Serial.println("conectado");
    mqttClient.publish("/test/one", 0, true, String(humidity).c_str());
  }
  delay(DELAY);
}