#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <AsyncMqttClient.h>
#include <string.h>

#define FC28_PIN A0
#define DELAY 5000
#define WIFI_SSID ""
#define WIFI_PASS ""
#define MQTT_HOST IPAddress(192, 168, 1, 12)
#define MQTT_PORT 1883
#define TOPIC "/test/one"
// h, min, sec
#define TIME 1 * 1 * 60 * 1e6

AsyncMqttClient mqttClient;

String createJSON(String hum, unsigned long time){
  String json = "{\"sensor\":\""+String(ESP.getChipId())+ "\", \"type\": \"FC-28\", \"value\": " + hum + ", \"time\": " + time + " }";
  return json;
}

String readFC28(uint8_t pin){
   int humidity = analogRead(pin);
   return String(humidity);
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("MQTT connected");

  if(mqttClient.connected()){
    String humidityFC28 = readFC28(FC28_PIN);
    String json = createJSON(humidityFC28, millis());
    mqttClient.publish(TOPIC, 1, false, json.c_str());
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged: " + packetId);
  ESP.deepSleep(TIME);
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

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.connect();
}

void loop() {}