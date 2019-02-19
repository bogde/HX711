#include "HX711.h"

// CIRCUIT:
// HX711 DOUT   TO PIN 2
// HX711 PD_SCK	TO PIN 3

HX711 scale;

void setup() {
  Serial.begin(57600);
  scale.begin(D2, D3);
}

void loop() {

  if (scale.wait_ready_retry(10)) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found.");
  }

  delay(1500);
  
}
