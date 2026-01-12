/*
  Created by Oskar Huotari, 10.1.2026
*/

#ifndef IMU_h
#define IMU_h

#include <Arduino.h>
#include <SPI.h>

class IMU
{
public:
  IMU();

  bool init(SPIClass &spi);       // initialize IMU, pass SPI bus
  bool works();                    // WHO_AM_I check

  float readTemperature();         // temperature in °C
  void readAccel(float &x, float &y, float &z); // g
  void readGyro(float &x, float &y, float &z);  // deg/s

private:
  SPIClass *_spi;
  SPISettings _spiSettings;

  // Sensitivities for conversions
  float _accSensitivity;
  float _gyroSensitivity;

  // SPI helpers
  uint8_t _spiRead(uint8_t reg);
  void _spiReadBytes(uint8_t reg, uint8_t *buffer, uint8_t length);
  void _spiWrite(uint8_t reg, uint8_t val);
  void _modifyRegister(uint8_t reg, uint8_t mask, uint8_t val);

  // CS pin is fixed by PCB
  static constexpr uint8_t CS_IMU = 16;

  // WHO_AM_I
  static constexpr uint8_t WHO_AM_I_REG = 0x75;
  static constexpr uint8_t WHO_AM_I_VAL = 0x3B;
};

#endif
