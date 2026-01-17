/*
  Tonttulib.h - Library for interfacing with Tonttuboard:
    a custom PCB designed for the Finnish 2026 Cansat competition.
  Created by Oskar Huotari, 2026
*/
#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "Constants.h"

// Sensors
#include "sensors/BMP388.h"
#include "sensors/IMU.h"
#include "sensors/Thermistor.h"
#include "sensors/LDR.h"

// Actuators
#include "actuators/LED.h"
#include "actuators/Motors.h"

// Storage
#include "storage/Flash.h"

class Tonttulib
{
public:
  Tonttulib();

  int init(TwoWire &wire = Wire, SPIClass &spi = SPI);
  void update();

  // Modules
  BMP388 baro;
  IMU imu;
  Thermistor thermistor;
  LDR ldr;
  Motors motors;
  Led led;
  Flash flash;

private:
  TwoWire *_i2c;
  SPIClass *_spi;
};
