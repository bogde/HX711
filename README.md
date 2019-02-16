# HX711
An Arduino library to interface the [Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC)] for Weight Scales.


## Synopsis
```c++
#include "HX711.h"
HX711 loadcell;

// 1. Loadcell settings

// AVR and friends
const int LOADCELL_DOUT_PIN = A1;
const int LOADCELL_SCK_PIN = A0;

// ESP and friends
const int LOADCELL_DOUT_PIN = D2;
const int LOADCELL_SCK_PIN = D3;

// 2. Adjustment settings
const long LOADCELL_OFFSET = 50682624;
const long LOADCELL_DIVIDER = 5895655;

// 3. Initialize library
loadcell.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
loadcell.set_scale(LOADCELL_DIVIDER);
loadcell.set_offset(LOADCELL_OFFSET);

// 4. Acquire reading
Serial.print("Weight: ");
Serial.println(loadcell.get_units(10), 2);
```


## Full example
See `examples/hx711_example.ino` in this repository.


## HAL support
- [Arduino AVR core](https://github.com/arduino/ArduinoCore-avr) (untested)
- [Arduino core for ESP8266](https://github.com/esp8266/Arduino) (untested)
- [Arduino core for ESP32](https://github.com/espressif/arduino-esp32) (untested)
- [Arduino core for SAMD21](https://github.com/arduino/ArduinoCore-samd) (untested)
- [Arduino core for SAMD21 and SAMD51](https://github.com/adafruit/ArduinoCore-samd) (untested)


## Features
1. It provides a `tare()` function, which "resets" the scale to 0. Many other
   implementations calculate the tare weight when the ADC is initialized only.
   I needed a way to be able to set the tare weight at any time.
   **Use case**: Place an empty container on the scale, call `tare()` to reset
   the readings to 0, fill the container and get the weight of the content.

2. It provides a `power_down()` function, to put the ADC into a low power mode.
   According to the datasheet,
   > When PD_SCK pin changes from low to high and stays at high
   > for longer than 60μs, HX711 enters power down mode.

   **Use case**: Battery-powered scales. Accordingly, there is a `power_up()`
   function to get the chip out of the low power mode.

3. It has a `set_gain(byte gain)` function that allows you to set the gain factor
   and select the channel. According to the datasheet,
   > Channel A can be programmed with a gain of 128 or 64, corresponding to
   a full-scale differential input voltage of ±20mV or ±40mV respectively, when
   a 5V supply is connected to AVDD analog power supply pin. Channel B has
   a fixed gain of 32.

   The same function is used to select the channel A or channel B, by passing
   128 or 64 for channel A, or 32 for channel B as the parameter. The default
   value is 128, which means "channel A with a gain factor of 128", so one can
   simply call `set_gain()`.

   This function is also called from the initializer method `begin()`.

4. The `get_value()` and `get_units()` functions can receive an extra parameter "times",
   and they will return the average of multiple readings instead of a single reading.


## How to calibrate your load cell
1. Call `set_scale()` with no parameter.
2. Call `tare()` with no parameter.
3. Place a known weight on the scale and call `get_units(10)`.
4. Divide the result in step 3 to your known weight. You should
   get about the parameter you need to pass to `set_scale()`.
5. Adjust the parameter in step 4 until you get an accurate reading.


## Deprecating warning
This library received some spring-cleaning in February 2019, removing
the pin definition within the constructor completely, as this was not
timing safe. (#29) Please use the new initialization flavor as outlined
in the example above.


## Credits
- This library is where everything started from
  https://github.com/aguegu/ardulibs/tree/master/hx711
- Contributions for supporting ESP32
  - https://github.com/lemio/HX711
  - https://github.com/bogde/HX711/issues/75
- Many contributions by others. Thanks!


## Similar libraries
- https://github.com/aguegu/ardulibs/tree/master/hx711
- https://github.com/olkal/HX711_ADC
- https://github.com/queuetue/Q2-HX711-Arduino-Library


[Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC)]: http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf
