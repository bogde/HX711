HX711
=====

An Arduino library to interface the Avia Semiconductor HX711 24-Bit Analog-to-Digital Converter (ADC) for Weight Scales.

This is my humble attempt at creating an Arduino library for this ADC:
http://www.dfrobot.com/image/data/SEN0160/hx711_english.pdf

Other libraries exist, including this very good one, which I first used and which is the starting point for my library:
https://github.com/aguegu/ardulibs/tree/master/hx711

Although other libraries exist, I needed a slightly different approach, so here's how my library is different than others:

1. It provides a tare() function, which "resets" the scale to 0. Many other implementations calculate the tare weight when the ADC is initialized only. I needed a way to be able to set the tare weight at any time. Use case: place an empty container on the scale, call tare() to reset the readings to 0, fill the container and get the weight of the content.

2. It provides a power_down() function, to put the ADC into a low power mode. According to the datasheet, "When PD_SCK pin changes from low to high and stays at high for longer than 60μs, HX711 enters power down mode". Use case: battery powered scales. Accordingly, there is a power_up() function to get the chip out of the low power mode.

3. It has a set_gain(byte gain) function that allows you to set the gain factor and select the channel. According to the datasheet, "Channel A can be programmed with a gain of 128 or 64, corresponding to a full-scale differential input voltage of ±20mV or ±40mV respectively, when a 5V supply is connected to AVDD analog power supply pin. Channel B has a fixed gain of 32.". The same function is used to select the channel A or channel B, by passing 128 or 64 for channel A, or 32 for channel B as the parameter. The default value is 128, which means "channel A with a gain factor of 128", so one can simply call set_gain(). Also, the function is called from the constructor.

4. The constructor has an extra parameter "gain" that allows you to set the gain factor and channel. The constructor calls the "set_gain" function mentioned above.

5. The "get_value" and "get_units" functions can receive an extra parameter "times", and they will return the average of multiple readings instead of a single reading.

How to Calibrate Your Scale

1. Call set_scale() with no parameter.
2. Call tare() with no parameter.
3. Place a known weight on the scale and call get_units(10).
4. Divide the result in step 3 to your known weight. You should get about the parameter you need to pass to set_scale.
5. Adjust the parameter in step 4 until you get an accurate reading.
