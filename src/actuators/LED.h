#ifndef LED_H
#define LED_H

#include <Arduino.h>

class Led
{
public:
  Led();

  void begin();

  void on();
  void off();
  void blinkSlow();
  void blinkFast();

  void update(); // must be called periodically

private:
  enum Mode
  {
    OFF,
    ON,
    BLINK_SLOW,
    BLINK_FAST
  };

  uint8_t _pin;
  Mode _mode;
  bool _state;
  unsigned long _lastToggle;
};

#endif
