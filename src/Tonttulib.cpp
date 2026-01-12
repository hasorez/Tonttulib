/*
  Tonttulib.cpp - Library for interfacing with Tonttuboard:
    a custom PCB designed by me for the Finnish 2026 CanSat competition.
  Created by Oskar Huotari, 10.1.2026
*/

#include "Arduino.h"
#include "Tonttulib.h"

Tonttulib::Tonttulib()
    : baro(), imu(), flash() {}

//  1  = success
// -1  = baro init failed
// -2  = IMU init failed
int Tonttulib::init(TwoWire &wire, SPIClass &spi)
{
    _i2c = &wire;
    _spi = &spi;

    // I2C init
    _i2c->begin();
    _i2c->setClock(400000);

    // SPI init
    _spi->begin();

    // Initialize BMP388
    if (!baro.init(*_i2c)) {
        return -1;
    }

    // Initialize IMU
    if (!imu.init(*_spi)) {
        return -2;
    }

    // Initialize Flash
    if (!flash.begin()) {
        return -3;
    }

    return 1;
}