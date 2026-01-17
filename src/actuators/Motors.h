#pragma once

#include <Arduino.h>
#include <Servo.h>

class Motors
{
public:
  void begin(const uint8_t pins[4]);
  void set(uint8_t index, uint16_t us);

private:
  Servo _motors[4];
};
