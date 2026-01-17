#include "Motors.h"

void Motors::begin(const uint8_t pins[4])
{
  for (uint8_t i = 0; i < 4; i++)
  {
    _motors[i].attach(pins[i]);
    _motors[i].writeMicroseconds(1000);
  }
}

// index: 1-4
void Motors::set(uint8_t index, uint16_t us)
{
  if (index >= 4)
    return;
  us = constrain(us, 1000, 2000);
  _motors[index - 1].writeMicroseconds(us);
}
