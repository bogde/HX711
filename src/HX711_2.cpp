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
#include "HX711_2.h"

// TEENSYDUINO has a port of Dean Camera's ATOMIC_BLOCK macros for AVR to ARM Cortex M3.
#define HAS_ATOMIC_BLOCK (defined(ARDUINO_ARCH_AVR) || defined(TEENSYDUINO))

// Whether we are running on either the ESP8266 or the ESP32.
#define ARCH_ESPRESSIF (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32))

// Whether we are actually running on FreeRTOS.
#define IS_FREE_RTOS defined(ARDUINO_ARCH_ESP32)

// Define macro designating whether we're running on a reasonable
// fast CPU and so should slow down sampling from GPIO.
#define FAST_CPU                                                 \
  (                                                              \
      ARCH_ESPRESSIF ||                                          \
      defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || \
      defined(ARDUINO_ARCH_STM32) || defined(TEENSYDUINO))

#if HAS_ATOMIC_BLOCK
// Acquire AVR-specific ATOMIC_BLOCK(ATOMIC_RESTORESTATE) macro.
#include <util/atomic.h>
#endif

// Defines the delay(us) between setting the HIGH/LOW modes of the clock pins
#define DELAY_MICROSECONDS 3

static inline void doubleWrite(uint8_t pin1, uint8_t pin2, bool level)
{
  digitalWrite(pin1, level);
  if (pin1 != 255)
    digitalWrite(pin2, level);
}

uint16_t shiftInSlow(uint8_t dataPin, uint8_t dataPin2, uint8_t clockPin, uint8_t clockPin2, uint8_t bitOrder)
{
  uint8_t value = 0;
  uint8_t value2 = 0;
  uint8_t i;

  for (i = 0; i < 8; ++i)
  {
    doubleWrite(clockPin, clockPin2, HIGH);
    delayMicroseconds(DELAY_MICROSECONDS);
    doubleWrite(clockPin, clockPin2, LOW);
    delayMicroseconds(DELAY_MICROSECONDS);
    if (bitOrder == LSBFIRST)
    {
      value |= digitalRead(dataPin) << i;
      value2 |= digitalRead(dataPin2) << i;
    }
    else
    {
      value |= digitalRead(dataPin) << (7 - i);
      value2 |= digitalRead(dataPin2) << (7 - i);
    }
  }

  return value2 << 8 | value;
}

#if ARCH_ESPRESSIF
// ESP8266 doesn't read values between 0x20000 and 0x30000 when DOUT is pulled up.
#define DOUT_MODE INPUT
#else
#define DOUT_MODE INPUT_PULLUP
#endif

HX711_2::HX711_2()
{
}

HX711_2::~HX711_2()
{
}

void HX711_2::begin(byte dout, byte dout2, byte pd_sck, byte pd_sck2, byte gain, unsigned char sck_mode)
{
  PD_SCK = pd_sck;
  PD_SCK2 = pd_sck2;
  DOUT = dout;
  DOUT2 = dout2;

  pinMode(PD_SCK, sck_mode);
  if (PD_SCK2 != 255)
    pinMode(PD_SCK2, sck_mode);
  pinMode(DOUT, DOUT_MODE);
  pinMode(DOUT2, DOUT_MODE);

  set_gain(gain);
}

bool HX711_2::is_ready()
{
  if (digitalRead(DOUT2) == HIGH)
  {
    return false;
  }

  return digitalRead(DOUT) == LOW;
}

void HX711_2::set_gain(byte gain)
{
  switch (gain)
  {
  case 128: // channel A, gain factor 128
    GAIN = 1;
    break;
  case 64: // channel A, gain factor 64
    GAIN = 3;
    break;
  case 32: // channel B, gain factor 32
    GAIN = 2;
    break;
  }
}

void HX711_2::read(long *readValues, unsigned long timeout)
{

  // Wait for the chip to become ready.
  if (!wait_ready_timeout(timeout))
  {
    return;
  }

  // Define structures for reading data into.
  unsigned long value = 0;
  uint8_t data[6] = {0};
  uint8_t filler = 0x00;

// Protect the read sequence from system interrupts.  If an interrupt occurs during
// the time the PD_SCK signal is high it will stretch the length of the clock pulse.
// If the total pulse time exceeds 60 uSec this will cause the HX711 to enter
// power down mode during the middle of the read sequence.  While the device will
// wake up when PD_SCK goes low again, the reset starts a new conversion cycle which
// forces DOUT high until that cycle is completed.
//
// The result is that all subsequent bits read by shiftIn() will read back as 1,
// corrupting the value returned by read().  The ATOMIC_BLOCK macro disables
// interrupts during the sequence and then restores the interrupt mask to its previous
// state after the sequence completes, insuring that the entire read-and-gain-set
// sequence is not interrupted.  The macro has a few minor advantages over bracketing
// the sequence between `noInterrupts()` and `interrupts()` calls.
#if HAS_ATOMIC_BLOCK
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {

#elif IS_FREE_RTOS
  // Begin of critical section.
  // Critical sections are used as a valid protection method
  // against simultaneous access in vanilla FreeRTOS.
  // Disable the scheduler and call portDISABLE_INTERRUPTS. This prevents
  // context switches and servicing of ISRs during a critical section.
  portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
  portENTER_CRITICAL(&mux);

#else
  // Disable interrupts.
  noInterrupts();
#endif

    // Pulse the clock pin 24 times to read the data.
    uint16_t readBits = shiftInSlow(DOUT, DOUT2, PD_SCK, PD_SCK2, MSBFIRST);
    data[2] = readBits & 0xFF;
    data[5] = readBits >> 8;
    readBits = shiftInSlow(DOUT, DOUT2, PD_SCK, PD_SCK2, MSBFIRST);
    data[1] = readBits & 0xFF;
    data[4] = readBits >> 8;
    readBits = shiftInSlow(DOUT, DOUT2, PD_SCK, PD_SCK2, MSBFIRST);
    data[0] = readBits & 0xFF;
    data[3] = readBits >> 8;

    // Set the channel and the gain factor for the next reading using the clock pin.
    for (unsigned int i = 0; i < GAIN; i++)
    {
      doubleWrite(PD_SCK, PD_SCK2, HIGH);
#if FAST_CPU
      delayMicroseconds(DELAY_MICROSECONDS);
#endif
      doubleWrite(PD_SCK, PD_SCK2, LOW);
#if FAST_CPU
      delayMicroseconds(DELAY_MICROSECONDS);
#endif
    }

#if IS_FREE_RTOS
    // End of critical section.
    portEXIT_CRITICAL(&mux);

#elif HAS_ATOMIC_BLOCK
}

#else
  // Enable interrupts again.
  interrupts();
#endif

    // Replicate the most significant bit to pad out a 32-bit signed integer
    if (data[2] & 0x80)
    {
      filler = 0xFF;
    }
    else
    {
      filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = (static_cast<unsigned long>(filler) << 24 | static_cast<unsigned long>(data[2]) << 16 | static_cast<unsigned long>(data[1]) << 8 | static_cast<unsigned long>(data[0]));

    readValues[0] = static_cast<long>(value) - OFFSET;

    if (data[5] & 0x80)
    {
      filler = 0xFF;
    }
    else
    {
      filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = (static_cast<unsigned long>(filler) << 24 | static_cast<unsigned long>(data[5]) << 16 | static_cast<unsigned long>(data[4]) << 8 | static_cast<unsigned long>(data[3]));

    readValues[1] = static_cast<long>(value) - OFFSET2;
  }

  void HX711_2::wait_ready(unsigned long delay_ms)
  {
    // Wait for the chip to become ready.
    // This is a blocking implementation and will
    // halt the sketch until a load cell is connected.
    while (!is_ready())
    {
      // Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
      // https://github.com/bogde/HX711/issues/73
      delay(delay_ms);
    }
  }

  bool HX711_2::wait_ready_retry(int retries, unsigned long delay_ms)
  {
    // Wait for the chip to become ready by
    // retrying for a specified amount of attempts.
    // https://github.com/bogde/HX711/issues/76
    int count = 0;
    while (count < retries)
    {
      if (is_ready())
      {
        return true;
      }
      delay(delay_ms);
      count++;
    }
    return false;
  }

  bool HX711_2::wait_ready_timeout(unsigned long timeout, unsigned long delay_ms)
  {
    // Wait for the chip to become ready until timeout.
    // https://github.com/bogde/HX711/pull/96
    unsigned long stopAt = millis() + timeout;
    while (millis() < stopAt)
    {
      if (is_ready())
      {
        return true;
      }
      delay(delay_ms);
    }
    return false;
  }

  void HX711_2::read_average(long *readValues, byte times)
  {
    long sum[2] = {0};
    for (byte i = 0; i < times; ++i)
    {
      long values[2];
      read(values);
      sum[0] += values[0];
      sum[1] += values[1];
    }
    readValues[0] = sum[0] / times;
    readValues[1] = sum[1] / times;
  }

  void HX711_2::get_value(long *readValues, byte times)
  {
    read_average(readValues, times);
  }

  void HX711_2::get_units(float *readValues, byte times)
  {
    long values[2];
    read_average(values, times);

    readValues[0] = values[0] / (SCALE == 0 ? 1 : SCALE);
    readValues[1] = values[1] / (SCALE2 == 0 ? 1 : SCALE2);
  }

  void HX711_2::tare(byte times)
  {
    long readValues[2];
    set_offset(0, 0);
    read_average(readValues, times);
    set_offset(readValues[0], readValues[1]);
  }

  void HX711_2::set_scale(float scale, float scale2)
  {
    SCALE = scale;
    SCALE2 = scale2;
  }

  void HX711_2::get_scale(float *scaleValues)
  {
    scaleValues[0] = SCALE;
    scaleValues[1] = SCALE2;
  }

  void HX711_2::set_offset(long offset, long offset2)
  {
    OFFSET = offset;
    OFFSET2 = offset2;
  }

  void HX711_2::get_offset(long *offsetValues)
  {
    offsetValues[0] = OFFSET;
    offsetValues[1] = OFFSET2;
  }

  void HX711_2::power_down()
  {
    doubleWrite(PD_SCK, PD_SCK2, LOW);
#if FAST_CPU
    delayMicroseconds(DELAY_MICROSECONDS);
#endif
    doubleWrite(PD_SCK, PD_SCK2, HIGH);
  }

  void HX711_2::power_up()
  {
    doubleWrite(PD_SCK, PD_SCK2, LOW);
  }
