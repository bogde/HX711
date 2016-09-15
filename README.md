HX711-multi
=====

This is a modification of bogde's (https://github.com/bogde/HX711) HX711 library for the Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter for scales (load cells) (ADC / LCA) [Datasheet: http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf]


# Advantages of this library
In its current form, it is stripped of a lot of functionality (like tare, unit conversion, etc.) though this functionality is easy to implement outside the library if neccessary, and will be added to this library in the future.

The main difference this library introduces is the ability to sample multiple HX711 units simultaneously. 
It is optimized for intoducing minimal (effectively zero) overhead.

# Wiring
Connect all HX711 units to share a single clock line, and plug each data line from the HX711 to a different pin on the Arduino 

# How to use
See the examples section for usage, as well as some useful tools for evaluating your setup.
