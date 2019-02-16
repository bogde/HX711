/**
 *
 * HX711 library for Arduino
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include <Arduino.h>
#include <HX711.h>

#if defined(ARDUINO) && ARDUINO <= 106
	// "yield" is not implemented as noop in older Arduino Core releases, so let's define it.
	// See also: https://stackoverflow.com/questions/34497758/what-is-the-secret-of-the-arduino-yieldfunction/34498165#34498165
	void yield(void) {};
#endif

#if defined(ESP_H) || defined(CORE_TEENSY)
// Make shiftIn() be aware of clockspeed for ESP32, Teensy 3.x and friends
// https://github.com/bogde/HX711/issues/75
// https://github.com/arduino/Arduino/issues/6561
// https://community.hiveeyes.org/t/using-bogdans-canonical-hx711-library-on-the-esp32/539
uint8_t shiftInSlow(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;

    for(i = 0; i < 8; ++i) {
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(1);
        if(bitOrder == LSBFIRST)
            value |= digitalRead(dataPin) << i;
        else
            value |= digitalRead(dataPin) << (7 - i);
        digitalWrite(clockPin, LOW);
        delayMicroseconds(1);
    }
    return value;
}
#define SHIFTIN_WITH_SPEED_SUPPORT(data,clock,order) shiftInSlow(data,clock,order)
#else
#define SHIFTIN_WITH_SPEED_SUPPORT(data,clock,order) shiftIn(data,clock,order)
#endif

HX711::HX711() {
}

HX711::~HX711() {
}

void HX711::begin(byte dout, byte pd_sck, byte gain) {
	PD_SCK = pd_sck;
	DOUT = dout;

	pinMode(PD_SCK, OUTPUT);
	pinMode(DOUT, INPUT);

	set_gain(gain);
}

bool HX711::is_ready() {
	return digitalRead(DOUT) == LOW;
}

void HX711::set_gain(byte gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

	digitalWrite(PD_SCK, LOW);
	read();
}

long HX711::read() {
	// wait for the chip to become ready
	while (!is_ready()) {
		// Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
		yield();
	}

	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	#ifdef ESP_H
	// Begin of critical section.
	// Critical sections are used as a valid protection method
	// against simultaneous access in vanilla FreeRTOS.
	// Disable the scheduler and call portDISABLE_INTERRUPTS. This prevents
	// context switches and servicing of ISRs during a critical section.
	portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
	portENTER_CRITICAL(&mux);
	#endif

	// pulse the clock pin 24 times to read the data
	data[2] = SHIFTIN_WITH_SPEED_SUPPORT(DOUT, PD_SCK, MSBFIRST);
	data[1] = SHIFTIN_WITH_SPEED_SUPPORT(DOUT, PD_SCK, MSBFIRST);
	data[0] = SHIFTIN_WITH_SPEED_SUPPORT(DOUT, PD_SCK, MSBFIRST);

	// set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = 0; i < GAIN; i++) {
		digitalWrite(PD_SCK, HIGH);
		#ifdef ESP_H
		delayMicroseconds(1);
		#endif
		digitalWrite(PD_SCK, LOW);
		#ifdef ESP_H
		delayMicroseconds(1);
		#endif
	}

	#ifdef ESP_H
	// End of critical section.
	portEXIT_CRITICAL(&mux);
	#endif

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( static_cast<unsigned long>(filler) << 24
			| static_cast<unsigned long>(data[2]) << 16
			| static_cast<unsigned long>(data[1]) << 8
			| static_cast<unsigned long>(data[0]) );

	return static_cast<long>(value);
}

long HX711::read_average(byte times) {
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += read();
		yield();
	}
	return sum / times;
}

double HX711::get_value(byte times) {
	return read_average(times) - OFFSET;
}

float HX711::get_units(byte times) {
	return get_value(times) / SCALE;
}

void HX711::tare(byte times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711::set_scale(float scale) {
	SCALE = scale;
}

float HX711::get_scale() {
	return SCALE;
}

void HX711::set_offset(long offset) {
	OFFSET = offset;
}

long HX711::get_offset() {
	return OFFSET;
}

void HX711::power_down() {
	digitalWrite(PD_SCK, LOW);
	digitalWrite(PD_SCK, HIGH);
}

void HX711::power_up() {
	digitalWrite(PD_SCK, LOW);
}
