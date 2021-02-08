#include <Arduino.h>
#include "ESP8266WiFi.h"
#include <AsyncMqttClient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <ESPDateTime.h>

#define FC28_PIN A0

static const unsigned long WAIT_DELAY = 5000;
static const int MAX_RETRY = 5;
// MQTT config
#define MQTT_HOST IPAddress(192, 168, 1, 112)
#define MQTT_PORT 1883
#define MQTT_TOPIC "/test/one"
// NTP server config
static const int NTP_TIMEZONE = 0;
static const char* NTP_SERVER = "europe.pool.ntp.org";
static const unsigned long NTP_TIMEOUT = 6e4;
// data send interval config
static const unsigned long SEND_DATA = 3600e6;
static const unsigned long HUNDRED = 1000;

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
    mqttClient.publish(MQTT_TOPIC, 1, false, json.c_str());
  }
}

void sleep() {
  unsigned long sleep = SEND_DATA-(millis()*HUNDRED);
  Serial.print("Deep sleep: ");
  Serial.println(sleep);
  ESP.deepSleep(sleep);
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged: " + packetId);
  sleep();
}

void connectToNtp(const char* server, const unsigned int timeout, const int timezone ) {
  DateTime.setTimeZone(timezone);
  DateTime.setServer(server);
  DateTime.begin(timeout);
  if (!DateTime.isTimeValid()) {
    Serial.println("Failed to connect with NTP Server");
    sleep();
  }else {
    Serial.println("Connected to NTP Server with date: " + DateTime.toUTCString());
  }
}
void connectToWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASS);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED && i < MAX_RETRY) {
      i++;
      Serial.print(i);
      Serial.println("- Wifi not connected...");
      delay(WAIT_DELAY);
    }
    if(i==MAX_RETRY){
      sleep();
    }
    Serial.print("Connected to Wifi with IP: ");
    Serial.println(WiFi.localIP());
}

void connectToMqtt(){
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.connect();

  int i = 0;
  while(!mqttClient.connected() && i<MAX_RETRY){
    i++;
    Serial.print(i);
    Serial.println("- Mqtt not connected...");
    delay(WAIT_DELAY);
  }
  if(i==MAX_RETRY){
    Serial.println("Failed to connect with Mqtt Server");
    sleep();
  }
}

void setup() {
  Serial.begin(9600);
  
  connectToWifi();

  connectToNtp(NTP_SERVER, NTP_TIMEOUT, NTP_TIMEZONE);

  connectToMqtt();
}

void loop() {
  delay(WAIT_DELAY);
}
