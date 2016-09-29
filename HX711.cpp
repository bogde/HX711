#include <Arduino.h>
#include "HX711.h"

HX711::HX711(byte dout, byte pd_sck, byte gain) {
	begin(dout, pd_sck, gain);
}

HX711::HX711() {
}

HX711::~HX711() {

}

void HX711::begin(byte dout, byte pd_sck, byte gain) {
	PD_SCK = pd_sck;
	DOUT   = dout;

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
    // Byte:     0        1        2        3
    // Bits:  76543210 76543210 76543210 76543210
    // Data: |--------|--------|--------|--------|
    // Bit#:  33222222 22221111 11111100 00000000
    //        10987654 32109876 54321098 76543210
    union DataBuffer {
        byte data[4];
        long value;
    } data_buffer;

    // Wait for the chip to become ready
    for (; !is_ready() ;) {
        // Will do nothing on Arduino but prevent resets of ESP8266 (Watchdog Issue)
        yield();
    }

    // Pulse the clock pin 24 times to read the data
    data_buffer.data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
    data_buffer.data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
    data_buffer.data[3] = shiftIn(DOUT, PD_SCK, MSBFIRST);

	// Set the channel and the gain factor for the next reading using the clock pin
	for (unsigned int i = GAIN ; 0 < i ; --i) {
		digitalWrite(PD_SCK, HIGH);
		digitalWrite(PD_SCK, LOW);
	}

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if ( data_buffer.data[1] & 0x80 ) {
        data_buffer.data[0] = 0xFF;
    } else {
        data_buffer.data[0] = 0x00;
    }

    // Datasheet indicates the value is a 24-bit two's complement (signed) value
    // https://cdn.sparkfun.com/datasheets/Sensors/ForceFlex/hx711_english.pdf

    // Flip all the bits
    data_buffer.value = ~data_buffer.value;

    // ... and add 1
    return ++data_buffer.value;
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
