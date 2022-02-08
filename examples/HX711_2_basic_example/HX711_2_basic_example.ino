#include "HX711_2.h"

// HX711 circuit wiring
const int LOADCELL_SCK_PIN = 2;
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_DOUT2_PIN = 4;

HX711_2 scale;

void setup() {
  Serial.begin(57600);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_DOUT2_PIN, LOADCELL_SCK_PIN);
}

void loop() {
  
  scale.power_up();

  if (scale.is_ready()) {
    long reading[2];
    
    scale.read(reading);
    
    Serial.print("HX711 readings: ");
    Serial.print(reading[0]);
    Serial.print(" ");
    Serial.println(reading[1]);
  } else {
    Serial.println("HX711 not found.");
  }
  
  scale.power_down();

  delay(1000);
}
