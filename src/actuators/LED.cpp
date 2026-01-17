#include "LED.h"

Led::Led()
{
  _pin = 23;
  _mode = OFF;
  _state = false;
  _lastToggle = 0;
}

void Led::begin()
{
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);

  _mode = OFF;
  _state = false;
  _lastToggle = millis();
}

void Led::on()
{
  _mode = ON;
  digitalWrite(_pin, HIGH);
}

void Led::off()
{
  _mode = OFF;
  digitalWrite(_pin, LOW);
}

void Led::blinkSlow()
{
  _mode = BLINK_SLOW;
}

void Led::blinkFast()
{
  _mode = BLINK_FAST;
}

void Led::update()
{
  unsigned long now = millis();
  unsigned long interval;

  switch (_mode)
  {
  case BLINK_SLOW:
    interval = 1000;
    break;

  case BLINK_FAST:
    interval = 200;
    break;

  default:
    return;
  }

  if (now - _lastToggle >= interval)
  {
    _lastToggle = now;
    _state = !_state;
    digitalWrite(_pin, _state ? HIGH : LOW);
  }
}
