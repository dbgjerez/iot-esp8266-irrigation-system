#include <Arduino.h>
#include "ESP8266WiFi.h"

#define FC28_PIN A0
#define DELAY 5000
#define WIFI_SSID ""
#define WIFI_PASS ""

static const int MIN_RANGE = 0;
static const int MAX_RANGE = 1023;
static const int CEN = 100;

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al wifi...");
  }

  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int humidity = analogRead(FC28_PIN);
  float humidityPercent = CEN - map(humidity, MIN_RANGE, MAX_RANGE, MIN_RANGE, CEN);
  Serial.print("Sensor: ");
  Serial.print(humidity);
  Serial.print(" - ");
  Serial.print(humidityPercent);
  Serial.println("%");
  delay(DELAY);
}