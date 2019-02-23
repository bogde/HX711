# HX711 library notes


## Backlog
- [o] Get library into https://www.arduinolibraries.info/ and https://platformio.org/
- [o] Maybe use constructor-based initialization again?
  It does not necessarily need to starting talking to the hardware yet!
- [o] Unify critical sections / interrupt disabling between platforms?
  https://github.com/esp8266/Arduino/issues/2218
- [o] Check out https://github.com/mmarchetti/DirectIO

### Scan more forks

- https://github.com/bigujun/HX711
- https://github.com/OpenCamper/HX711
- https://github.com/compugician/HX711-multi
- https://github.com/CasualTriangle/HX711-multi
- https://github.com/joeybane/HX711-multi
- https://github.com/polmes/HX711-multi
- https://github.com/knifter/HX711

See also

- https://github.com/newAM/LoadCellOccupany


### Add links to pin mappings of popular chips
- https://stackoverflow.com/questions/42022000/which-pins-should-i-take-for-i2c-on-arduino-uno/42022566
- https://www.arduino.cc/en/Hacking/PinMapping32u4
- https://www.arduino.cc/en/Hacking/PinMappingSAM3X
- https://www.avdweb.nl/arduino/samd21/samd21-variant
- https://techtutorialsx.com/2017/04/02/esp8266-nodemcu-pin-mappings/
- https://github.com/esp8266/Arduino/issues/584
- https://www.arduino.cc/en/Hacking/Atmega168Hardware
- https://www.arduino.cc/en/Hacking/PinMapping168


---


# Spring-cleaning issue summary

https://github.com/hiveeyes/HX711/tree/spring-cleaning

## AVR
- [x] AVR interrupt safety
  https://github.com/bogde/HX711/pull/62

## ARM/SAMD

### Teensy 3.x
- [x] Thomas O Fredericks
  https://github.com/bogde/HX711/pull/96

### Arduino Due
- [x] Drop a line at https://github.com/aguegu/ardulibs/issues/3 re. support for Arduino Due

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
- [o] https://github.com/bogde/HX711/issues/67
- [x] https://github.com/bogde/HX711/issues/73
- [x] https://github.com/bogde/HX711/pull/81
- [x] https://github.com/bogde/HX711/pull/86
- [x] https://github.com/bogde/HX711/issues/120
- [x] https://github.com/bogde/HX711/issues/101
- [o] https://github.com/Skaronator/ESP8266-Load-Cell/issues/6
- [x] Q: Would `delay(1)` be better than `delay(0)`?
      A: even delay(0) will work. Should be as often as you can spare, but not more than 100ms let's say
         -- https://github.com/esp8266/Arduino/issues/2240#issuecomment-230874704

### ESP8266 lacking pin mapping
- [x] https://github.com/bruhautomation/ESP-MQTT-JSON-Multisensor/issues/14
- [x] https://github.com/witnessmenow/simple-arduino-crypto-display/issues/2
- [x] https://github.com/wemos/D1_mini_Examples/issues/21
- [x] https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

### ESP32 too fast
- [x] https://github.com/lemio/HX711
- [x] https://github.com/bogde/HX711/issues/75
