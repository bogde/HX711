#include "HX711.h"

// CIRCUIT:
// HX711 DOUT TO PIN 2 
// HX711 PD_SCK	TO PIN 3

HX711 scale;

void setup() {
  Serial.begin(57600);
  scale.begin(A2, A3);
}

void loop() {

  // Variables for the timeout
  unsigned long millisStarted = millis();
  bool timeoutHappened = false;

  // Wait up to 1000 milliseconds until the scale is ready
  while ( scale.is_ready() == false ) {
    // Do nothing while we are waiting for the scale...
    // Stop waiting if we waited at least 1000 milliseconds:
     if ( millis() - millisStarted >= 1000 ) {
      timeoutHappened = true;
      break;
     }
  } 

  if ( timeoutHappened  ) {
    Serial.println("HX711 was not found.");
  } else {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  }

  delay(1500);
  
}
