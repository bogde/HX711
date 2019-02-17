# HX711 library issue summary


## Miscellaneous
- [] Get library into https://www.arduinolibraries.info/ and https://platformio.org/
- [] Maybe use constructor-based initialization again?
- [] Unify critical sections / interrupt disabling between platforms?
  https://github.com/esp8266/Arduino/issues/2218
- [] Would `delay(1)` be better than `delay(0)`?
- [] Pulling all together
  https://github.com/hiveeyes/HX711/tree/spring-cleaning


## AVR
- [x] AVR interrupt safety
  https://github.com/bogde/HX711/pull/62



## ARM/SAMD

### Arduino Due
- [] https://github.com/aguegu/ardulibs/issues/3

### Teensy 3.x
- [x] Thomas O Fredericks
  https://github.com/bogde/HX711/pull/96


## Espressif

### ESP8266 arch pragma / yield definition woes
- [x] https://github.com/bogde/HX711/issues/119
- [x] https://github.com/bogde/HX711/issues/114

### ESP8266 constructor initialization freezes
- [x] https://github.com/bogde/HX711/issues/29
- [x] https://github.com/bogde/HX711/pull/40
- [x] https://github.com/bogde/HX711/pull/113
- [x] https://github.com/bogde/HX711/pull/53
- [x] https://github.com/bogde/HX711/pull/122
- [x] https://github.com/bogde/HX711/issues/89

### ESP8266 WDT
- [x] https://github.com/bogde/HX711/issues/73
- [x] https://github.com/bogde/HX711/pull/81
- [x] https://github.com/bogde/HX711/pull/86
- [x] https://github.com/bogde/HX711/issues/120
- [x] https://github.com/bogde/HX711/issues/101

### ESP8266 lacking pin mapping
- [x] https://github.com/bruhautomation/ESP-MQTT-JSON-Multisensor/issues/14
- [x] https://github.com/witnessmenow/simple-arduino-crypto-display/issues/2
- [x] https://github.com/wemos/D1_mini_Examples/issues/21
- [x] https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h


### ESP32 too fast
- [x] https://github.com/lemio/HX711
- [x] https://github.com/bogde/HX711/issues/75
