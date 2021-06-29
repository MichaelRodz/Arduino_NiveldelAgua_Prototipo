#include <Wire.h>

#define tds A0


unsigned long last_time = 0;
unsigned long timer_delay = 5000;


void setup() {
  delay(1000);
  Serial.begin(9600);
}

void loop() {
  if ((millis() - last_time) > timer_delay) {
    float voltage_tds = analogRead(tds) * (float)5 / 1024;

    float tds_value = 2000 / 5 * voltage_tds;

    Serial.println("Voltage: ");
    Serial.println(voltage_tds);
    Serial.println("PPM: ");
    Serial.println(tds_value);

  last_time = millis();
  }
}
