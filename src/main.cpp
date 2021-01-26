#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <AsyncMqttClient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <ESPDateTime.h>

#define FC28_PIN A0
#define DELAY 5000
#define WIFI_SSID ""
#define WIFI_PASS ""
#define MQTT_HOST IPAddress(192, 168, 1, 12)
#define MQTT_PORT 1883
#define TOPIC "/test/one"
#define NTP_TIMEZONE 0
#define NTP_SERVER "europe.pool.ntp.org"
// 60.000ms = 60s = 1'
#define NTP_TIMEOUT 6e4
// sec
#define TIME 3600 * 1e6

static unsigned long HUNDRED = 1000;

AsyncMqttClient mqttClient;

String createJSON(int hum, unsigned long time){
  StaticJsonDocument<128> doc;
  doc[String("chip")]=String(ESP.getChipId());
  JsonObject sensors_0 = doc["sensors"].createNestedObject();
  sensors_0["sensor"] = "FC-28";
  sensors_0["time"] = time;
  sensors_0["humidity"] = hum;
  String json;
  serializeJson(doc, json);
  return json;
}

int readFC28(uint8_t pin){
   int humidity = analogRead(pin);
   return humidity;
}

void onMqttConnect(bool sessionPresent) {
  Serial.println("MQTT connected");

  if(mqttClient.connected()){
    int humidityFC28 = readFC28(FC28_PIN);
    String json = createJSON(humidityFC28, DateTime.utcTime());
    mqttClient.publish(TOPIC, 1, false, json.c_str());
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged: " + packetId);
  unsigned long sleep = TIME-(millis()*HUNDRED);
  Serial.println(sleep);
  ESP.deepSleep(sleep);
}

void initTimer(const char* server, const unsigned int timeout, const int timezone ) {
  DateTime.setTimeZone(timezone);
  DateTime.setServer(server);
  DateTime.begin(timeout);
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to get time from server.");
  }else {
    Serial.println("Fecha: " + DateTime.toUTCString());
  }
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

  initTimer(NTP_SERVER, NTP_TIMEOUT, NTP_TIMEZONE);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.connect();
}

void loop() {}