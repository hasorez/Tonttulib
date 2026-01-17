#include "LDR.h"
#include "Constants.h"

void LDR::begin(uint8_t pin)
{
  _pin = pin;
}

float LDR::readVoltage()
{
  return (analogRead(_pin) / ADC_MAX) * VCC;
}
