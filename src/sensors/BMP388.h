/*
  Created by Oskar Huotari, 10.1.2026
*/

#ifndef BMP388_h
#define BMP388_h

#include <Wire.h>
#include <Arduino.h>

class BMP388
{
public:
  BMP388();

  bool init(TwoWire &wire);
  bool works();
  float readTemperature();
  float readPressure();

private:
  TwoWire *_wire;

  struct _CalibrationData
  {
    // Temperature calibration
    double parT1;
    double parT2;
    double parT3;

    // Pressure calibration
    double parP1;
    double parP2;
    double parP3;
    double parP4;
    double parP5;
    double parP6;
    double parP7;
    double parP8;
    double parP9;
    double parP10;
    double parP11;

    float tLin;
  };

  _CalibrationData _calib;

  // Low-level I2C helpers
  void _writeRegister(uint8_t reg, uint8_t val);
  uint8_t _readRegister(uint8_t reg);
  void _modifyRegister(uint8_t reg, uint8_t mask, uint8_t val);

  void _readCalibrationData();
  bool _pressureReady();
  bool _tempReady();

  uint32_t _readRawTemperature();
  uint32_t _readRawPressure();

  float _compensateTemperature(uint32_t raw);
  float _compensatePressure(uint32_t raw);
};

#endif