/*
  Tonttulib.h - Library for interfacing with Tonttuboard:
    a custom PCB designed for the Finnish 2026 Cansat competition.
  Created by Oskar Huotari, 2026
*/

#ifndef Tonttulib_h
#define Tonttulib_h

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "sensors/BMP388.h"
#include "sensors/IMU.h"
#include "sensors/Flash.h"

class Tonttulib
{
public:
    Tonttulib();

    // Initialize all sensors
    // Returns 1 if successful, -1 if baro fails, -2 if IMU fails, -3 if flash fails
    int init(TwoWire &wire = Wire, SPIClass &spi = SPI);

    // Sensors
    BMP388 baro;
    IMU imu;
    Flash flash;

private:
    TwoWire* _i2c;
    SPIClass* _spi;
};

#endif
