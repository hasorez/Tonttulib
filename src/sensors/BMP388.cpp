#include "BMP388.h"
#include <Wire.h>
#include <Arduino.h>

#define BMP388_ADDR 0x76
#define PRESS_XLSB_REG 0x04
#define TEMP_XLSB_REG 0x07
#define STATUS_REG 0x03

BMP388::BMP388()
    : _wire(nullptr) {}

void BMP388::_writeRegister(uint8_t reg, uint8_t value)
{
  _wire->beginTransmission(BMP388_ADDR);
  _wire->write(reg);
  _wire->write(value);
  _wire->endTransmission();
}

uint8_t BMP388::_readRegister(uint8_t reg)
{
  _wire->beginTransmission(BMP388_ADDR);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom((uint8_t)BMP388_ADDR, (uint8_t)1);

  return _wire->read();
}

bool BMP388::works()
{
  // register 0x00 is chip id
  if (_readRegister(0x00) == 0x50)
    return true;

  return false;
}

// derived from sections 3.11.1 and 9.1
// https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp388-ds001.pdf
void BMP388::_readCalibrationData()
{

  uint16_t rawT1 = (uint16_t)((_readRegister(0x32) << 8) | _readRegister(0x31));
  _calib.parT1 = rawT1 * 256.0; // division by 2^-8

  uint16_t rawT2 = (uint16_t)((_readRegister(0x34) << 8) | _readRegister(0x33));
  _calib.parT2 = rawT2 / 1073741824.0; // division by 2^30

  _calib.parT3 = ((int8_t)_readRegister(0x35)) / 281474976710656.0; // division by 2^48

  int16_t rawP1 = (int16_t)((_readRegister(0x37) << 8) | _readRegister(0x36));
  _calib.parP1 = (rawP1 - 16384.0) / 1048576.0; // subtract 2^14 and divide by 2^20

  int16_t rawP2 = (int16_t)((_readRegister(0x39) << 8) | _readRegister(0x38));
  _calib.parP2 = (rawP2 - 16384.0) / 536870912.0; // subtract 2^14 and divide by 2^29

  _calib.parP3 = ((int8_t)_readRegister(0x3A)) / 4294967296.0; // division by 2^32

  _calib.parP4 = ((int8_t)_readRegister(0x3B)) / 137438953472.0; // division by 2^37

  uint16_t rawP5 = (uint16_t)((_readRegister(0x3D) << 8) | _readRegister(0x3C));
  _calib.parP5 = rawP5 * 8; // division by 2^-3

  uint16_t rawP6 = (uint16_t)((_readRegister(0x3F) << 8) | _readRegister(0x3E));
  _calib.parP6 = rawP6 / 64.0; // division by 2^6

  _calib.parP7 = ((int8_t)_readRegister(0x40)) / 256.0; // division by 2^8

  _calib.parP8 = ((int8_t)_readRegister(0x41)) / 32768.0; // division by 2^15

  int16_t rawP9 = (int16_t)((_readRegister(0x43) << 8) | _readRegister(0x42));
  _calib.parP9 = rawP9 / 281474976710656.0; // division by 2^48

  _calib.parP10 = ((int8_t)_readRegister(0x44)) / 281474976710656.0; // division by 2^48

  _calib.parP11 = ((int8_t)_readRegister(0x45)) / 36893488147419103232.0; // division by 2^65
}

void BMP388::_modifyRegister(uint8_t regAddr, uint8_t mask, uint8_t newVal)
{
  uint8_t reg = _readRegister(regAddr);
  // set all bits, that in mask are 1, to the value in newVal
  _writeRegister(regAddr, (reg & ~mask) | (newVal & mask));
}

bool BMP388::_pressureReady()
{
  return _readRegister(STATUS_REG) & 0x20; // bit 5 = drdy_press
}

// Wait for pressure data ready
bool BMP388::_tempReady()
{
  return _readRegister(STATUS_REG) & 0x40; // bit 6 = drdy_temp
}

// from BMP388_DEV lib
uint32_t BMP388::_readRawPressure()
{
  if (!_pressureReady())
  {
    return 0;
  }

  _wire->beginTransmission(BMP388_ADDR);
  _wire->write(PRESS_XLSB_REG); // start at XLSB
  _wire->endTransmission(false);
  _wire->requestFrom((uint8_t)BMP388_ADDR, (uint8_t)3);
  if (_wire->available() < 3)
    return 0;                   // sanity check
  uint8_t xlsb = _wire->read(); // 0x04
  uint8_t lsb = _wire->read();  // 0x05
  uint8_t msb = _wire->read();  // 0x06

  // Assemble 20-bit raw pressure
  int32_t raw = ((int32_t)msb << 16) | ((int32_t)lsb << 8) | ((int32_t)xlsb);

  return raw;
}

// from BMP388_DEV lib
uint32_t BMP388::_readRawTemperature()
{
  if (!_tempReady())
  {
    return 0;
  }

  _wire->beginTransmission(BMP388_ADDR);
  _wire->write(TEMP_XLSB_REG); // start at TEMP_XLSB
  _wire->endTransmission(false);
  _wire->requestFrom(BMP388_ADDR, (uint8_t)3);

  if (_wire->available() < 3)
    return 0;                   // sanity check
  uint8_t xlsb = _wire->read(); // 0x07
  uint8_t lsb = _wire->read();  // 0x08
  uint8_t msb = _wire->read();  // 0x09

  // Assemble 20-bit raw temperature
  int32_t raw = ((int32_t)msb << 16) | ((int32_t)lsb << 8) | ((int32_t)xlsb);

  return raw; // raw 20-bit temperature
}

// from the bosch datasheet section 9.2
float BMP388::_compensateTemperature(uint32_t uncompTemp)
{

  float partialData1;
  float partialData2;

  partialData1 = (float)(uncompTemp - _calib.parT1);
  partialData2 = (float)(partialData1 * _calib.parT2);

  /* Update the compensated temperature in calib structure since this is
   * needed for pressure calculation */
  _calib.tLin = partialData2 + (partialData1 * partialData1) * _calib.parT3;
  /* Returns compensated temperature */

  return _calib.tLin;
}

// from the bosch datasheet section 9.3
float BMP388::_compensatePressure(uint32_t uncompPress)
{

  /* Variable to store the compensated pressure */
  float compPress;

  /* Temporary variables used for compensation */
  float partialData1;
  float partialData2;
  float partialData3;
  float partialData4;
  float partialOut1;
  float partialOut2;

  /* Calibration data */
  partialData1 = _calib.parP6 * _calib.tLin;
  partialData2 = _calib.parP7 * (_calib.tLin * _calib.tLin);
  partialData3 = _calib.parP8 * (_calib.tLin * _calib.tLin * _calib.tLin);
  partialOut1 = _calib.parP5 + partialData1 + partialData2 + partialData3;

  partialData1 = _calib.parP2 * _calib.tLin;
  partialData2 = _calib.parP3 * (_calib.tLin * _calib.tLin);
  partialData3 = _calib.parP4 * (_calib.tLin * _calib.tLin * _calib.tLin);
  partialOut2 = (float)uncompPress * (_calib.parP1 + partialData1 + partialData2 + partialData3);

  partialData1 = (float)uncompPress * (float)uncompPress;
  partialData2 = _calib.parP9 + _calib.parP10 * _calib.tLin;
  partialData3 = partialData1 * partialData2;
  partialData4 = partialData3 + ((float)uncompPress * (float)uncompPress * (float)uncompPress) * _calib.parP11;

  compPress = partialOut1 + partialOut2 + partialData4;

  return compPress;
}

bool BMP388::init(TwoWire &wire)
{
  _wire = &wire;

  if (!works())
  {
    return false;
  }

  _readCalibrationData();

  // PWR_CTRL (0x1B):
  //* Bit 0:                          1
  //* Bit 1:                          1
  //* Bit 5..4:                       11
  _modifyRegister(0x1B, 0b00110011, 0b00110011);

  // OSR (0x1C):
  //* Bit 2..0:                      011
  //* Bit 5..3:                      000
  _modifyRegister(0x1C, 0b00111111, 0b00000011);

  // ODR (0x1D):
  //* Bit: 4..0:                     00010
  _modifyRegister(0x1D, 0b00011111, 0b00000010);

  // IIR 0x1F:
  //* Bit 3..1:                      010
  _modifyRegister(0x1F, 0b00001110, 0b00000100);
  return true;
}

// returns temperature in °C
float BMP388::readTemperature()
{
  uint32_t rawTemp = _readRawTemperature();
  if (rawTemp == 0)
    return NAN;
  return _compensateTemperature(rawTemp);
}

// returns pressure in Pa
float BMP388::readPressure()
{
  uint32_t rawPres = _readRawPressure();
  if (rawPres == 0 || !_tempReady())
    return NAN;
  readTemperature();
  return _compensatePressure(rawPres);
}
