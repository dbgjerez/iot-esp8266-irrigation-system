#include <Arduino.h>

#define FC28_PIN A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int humidity = analogRead(FC28_PIN);
  float humidityPercent = 100 - map(humidity, 0, 1023, 0, 100);
  Serial.print("Sensor: ");
  Serial.print(humidity);
  Serial.print(" - ");
  Serial.print(humidityPercent);
  Serial.println("%");
  delay(5000);
}