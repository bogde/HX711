#include "HX711.h"

HX711 scale;
void loop_test_hx711() {
	int which = 0;
  uint32_t preMillis = 0L;
	while (which < 5) {
		//Power cycle takes around 400ms so only do so if our test rate is greater than 500ms
		if ((millis() - preMillis) >= 5000) {
			switch (which) {
				case 0:
				case 2:
				case 4:
					scale.power_down();
					Serial.print(F("powerDownScale\n"));
					delay(1000);
					Serial.print(F("powerUpScale\n"));
					scale.power_up();
				break;
				case 1:
					scale.set_gain(128);
					Serial.print(F("set_gain=128\n"));
				break;
				case 3:
					scale.set_gain(64);
					Serial.print(F("set_gain=64\n"));
				break;
				default:
					which = 0;
				break;
			}
			preMillis = millis();
			which++;
		}

		float currentReading = 0;
		//Print raw reading
		long rawReading = scale.read();
		Serial.print(millis());
		Serial.print(F(","));
		Serial.print(rawReading, HEX);
		Serial.print(F(",\t"));
		//Take average of readings with calibration and tare taken into account
		currentReading = scale.get_units(1);
		Serial.print(millis());
		Serial.print(F(","));
		//Print calibrated reading
		Serial.print(currentReading, 3);
		Serial.println();
	}
	Serial.println(F("Loop test is done!"));
}
void setup() {
  Serial.begin(38400);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");
  // parameter "gain" is ommited; the default value 128 is used by the library
  // HX711.DOUT	- pin #A1
  // HX711.PD_SCK	- pin #A0
  scale.begin(A1, A0);
	loop_test_hx711();

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided 
						// by the SCALE parameter (not set yet)  

  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided 
						// by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);

  scale.power_down();			        // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
