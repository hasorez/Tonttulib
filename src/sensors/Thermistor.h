#pragma once
#include <Arduino.h>

class Thermistor
{
public:
  void begin(uint8_t pin);
  float readCelsius();

private:
  uint8_t _pin;
};
