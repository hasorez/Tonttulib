/*
  Tonttulib.cpp - Library for interfacing with Tonttuboard:
    a custom PCB designed by me for the Finnish 2026 CanSat competition.
  Created by Oskar Huotari, 10.1.2026
*/

#include "Tonttulib.h"

Tonttulib::Tonttulib()
    : baro(), imu(), thermistor(), ldr(), motors(), led(), flash()
{
}

int Tonttulib::init(TwoWire &wire, SPIClass &spi)
{
    _i2c = &wire;
    _spi = &spi;

    _i2c->begin();
    _i2c->setClock(400000);
    _spi->begin();

    if (!baro.init(*_i2c))
        return -1;
    if (!imu.init(*_spi))
        return -2;
    if (!flash.begin())
        return -3;

    thermistor.begin(PIN_THERMISTOR);
    ldr.begin(PIN_LDR);

    const uint8_t motorPins[4] = {
        MOTOR_1_PIN,
        MOTOR_2_PIN,
        MOTOR_3_PIN,
        MOTOR_4_PIN};
    motors.begin(motorPins);

    led.begin();

    return 1;
}

void Tonttulib::update()
{
    led.update();
}
