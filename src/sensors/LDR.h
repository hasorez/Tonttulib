#pragma once

#include <Arduino.h>

class LDR
{
public:
  void begin(uint8_t pin);
  float readVoltage();

private:
  uint8_t _pin;
};
