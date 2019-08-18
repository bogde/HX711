# HX711
An Arduino library to interface the [Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC)]
for reading load cells / weight scales.

It supports the architectures `atmelavr`, `espressif8266`, `espressif32`,
`atmelsam`, `teensy` and `ststm32` by corresponding [PlatformIO] targets.

[Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC)]: http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf
[PlatformIO]: https://platformio.org/


## Synopsis

### Blocking mode
The library is usually used in blocking mode, i.e. it will wait for the
hardware becoming available before returning a reading.

```c++
#include "HX711.h"
HX711 loadcell;

// 1. HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

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

### Non-blocking mode
It is also possible to define a maximum timeout to wait for the hardware
to be initialized. This won't send the program into a spinlock when the
scale is disconnected and will probably also account for hardware failures.
```
// 4. Acquire reading without blocking
if (loadcell.wait_ready_timeout(1000)) {
    long reading = loadcell.get_units(10);
    Serial.print("Weight: ");
    Serial.println(reading, 2);
} else {
    Serial.println("HX711 not found.");
}
```


## More examples
See `examples` directory in this repository.


## HAL support
- [Arduino AVR core](https://github.com/arduino/ArduinoCore-avr)
- [Arduino core for ESP8266](https://github.com/esp8266/Arduino)
- [Arduino core for ESP32](https://github.com/espressif/arduino-esp32)
- [Arduino core for SAMD21](https://github.com/arduino/ArduinoCore-samd) (untested)
- [Arduino core for SAMD51](https://github.com/adafruit/ArduinoCore-samd) (untested)
- [Arduino core for STM32](https://github.com/stm32duino/Arduino_Core_STM32)


## Hardware support
The library has been tested successfully on the following hardware.

- [ATmega328]: Arduino Uno
- [ESP8266]: WeMos D1 mini, Adafruit HUZZAH
- [ESP32]: ESP32 DEVKIT V1, Heltec WiFi Kit 32, Adafruit Feather HUZZAH32
- [STM32 F1] ([Cortex-M3]): STM32F103C8T6 STM32 Blue Pill Board

Thanks, @bogde and @ClemensGruber!

[ATmega328]: https://en.wikipedia.org/wiki/ATmega328
[ESP8266]: https://en.wikipedia.org/wiki/ESP8266
[ESP32]: https://en.wikipedia.org/wiki/ESP32
[STM32 F1]: https://en.wikipedia.org/wiki/STM32#STM32_F1
[Cortex-M3]: https://en.wikipedia.org/wiki/ARM_Cortex-M#Cortex-M3


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


## Build

### All architectures
This will spawn a Python virtualenv in the current directory,
install `platformio` into it and then execute `platformio run`,
effectively building for all environments defined in `platformio.ini`.

    make build-all

#### Result
```
Environment feather_328	[SUCCESS]
Environment atmega_2560	[SUCCESS]
Environment huzzah     	[SUCCESS]
Environment lopy4      	[SUCCESS]
Environment teensy31   	[SUCCESS]
Environment teensy36   	[SUCCESS]
Environment feather_m0 	[SUCCESS]
Environment arduino_due	[SUCCESS]
Environment feather_m4 	[SUCCESS]
Environment bluepill   	[SUCCESS]
```

#### Details
https://gist.github.com/amotl/5ed6b3eb1fcd2bc78552b218b426f6aa


### Specific architecture
You can run a build for a specific architecture by specifying
the appropriate platform label on the command line.

    # Build for LoPy4
    make build-env environment=lopy4

    # Build for Feather M0
    make build-env environment=feather_m0


## Deprecation warning
This library received some spring-cleaning in February 2019 (#123),
removing the pin definition within the constructor completely, as
this was not timing safe. (#29) Please use the new initialization
flavor as outlined in the example above.


## Credits
Thanks to Weihong Guan who started the first version of this library in 2012
already (see [[arduino|module]Hx711 electronic scale kit](http://aguegu.net/?p=1327),
[sources](https://github.com/aguegu/ardulibs/tree/master/hx711)), Bogdan Necula
who took over in 2014 and last but not least all others who contributed to this
library over the course of the last years, see also `CONTRIBUTORS.rst` in this
repository.

#### See also
- https://item.taobao.com/item.htm?id=18121631630
- https://item.taobao.com/item.htm?id=544769386300


## Similar libraries
There are other libraries around, enjoy:

- https://github.com/olkal/HX711_ADC
- https://github.com/queuetue/Q2-HX711-Arduino-Library


---

## Appendix

### Considerations about real world effects caused by physics
You should consider getting into the details of strain-gauge load cell
sensors when expecting reasonable results. The range of topics is from
sufficient and stable power supply, using the proper excitation voltage
to the Seebeck effect and temperature compensation.

See also:
- [Overview about real world effects](https://community.hiveeyes.org/t/analog-vs-digital-signal-gain-amplifiers/380/6)
- [Thermoelectric effect](https://en.wikipedia.org/wiki/Thermoelectric_effect) (Seebeck effect)
- Temperature compensation: [Resource collection](https://community.hiveeyes.org/t/temperaturkompensation-fur-waage-hardware-firmware/115), [DIY research](https://community.hiveeyes.org/t/temperaturkompensation-fur-waage-notig-datensammlung/245)
- [Power management for HX711](https://community.hiveeyes.org/t/stromversorgung-hx711/893)
