#include "Thermistor.h"
#include "Constants.h"
#include <math.h>

void Thermistor::begin(uint8_t pin)
{
  _pin = pin;
}

/**
 * Read temperature from an NTC thermistor.
 *
 * Circuit:
 *  - Voltage divider: thermistor + known resistor R2
 *  - Thermistor connected to VCC, R2 to GND
 *  - ADC measures voltage at the divider midpoint (UPin)
 *
 * Method:
 *  1. Convert ADC reading to voltage (UPin)
 *  2. Compute thermistor resistance using Ohm's law
 *     R_therm = R2 * (VCC - UPin) / UPin
 *  3. Convert resistance to temperature using the Beta equation
 *
 * Returns:
 *  - Temperature in degrees Celsius
 */
float Thermistor::readCelsius()
{
  float v = (analogRead(_pin) / ADC_MAX) * VCC;
  if (v <= 0.001f)
    return NAN;

  float r = R2 * (VCC - v) / v;
  float tK = 1.0f / (1.0f / T0 + log(r / R0) / BETA);
  return tK - 273.15f;
}
