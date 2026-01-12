#include "IMU.h"

IMU::IMU()
    : _spi(nullptr),
      _spiSettings(1000000, MSBFIRST, SPI_MODE0),
      _accSensitivity(4096.0f),
      _gyroSensitivity(32.8f)
{
}

uint8_t IMU::_spiRead(uint8_t reg)
{
    _spi->beginTransaction(_spiSettings);
    digitalWrite(CS_IMU, LOW);
    _spi->transfer(reg | 0x80);
    uint8_t val = _spi->transfer(0x00);
    digitalWrite(CS_IMU, HIGH);
    _spi->endTransaction();
    return val;
}

void IMU::_spiReadBytes(uint8_t reg, uint8_t *buffer, uint8_t length)
{
    _spi->beginTransaction(_spiSettings);
    digitalWrite(CS_IMU, LOW);
    _spi->transfer(reg | 0x80);
    for (uint8_t i = 0; i < length; i++)
        buffer[i] = _spi->transfer(0x00);
    digitalWrite(CS_IMU, HIGH);
    _spi->endTransaction();
}

void IMU::_spiWrite(uint8_t reg, uint8_t val)
{
    _spi->beginTransaction(_spiSettings);
    digitalWrite(CS_IMU, LOW);
    _spi->transfer(reg & 0x7F);
    _spi->transfer(val);
    digitalWrite(CS_IMU, HIGH);
    _spi->endTransaction();
}

void IMU::_modifyRegister(uint8_t reg, uint8_t mask, uint8_t val)
{
    uint8_t current = _spiRead(reg);
    _spiWrite(reg, (current & ~mask) | (val & mask));
}

// --- Public API ---
bool IMU::works()
{
    return _spiRead(WHO_AM_I_REG) == WHO_AM_I_VAL;
}

bool IMU::init(SPIClass &spi)
{
    _spi = &spi;

    pinMode(CS_IMU, OUTPUT);
    digitalWrite(CS_IMU, HIGH);

    _spi->begin();

    if (!works())
        return false;

    // Set default ranges
    _accSensitivity = 4096.0f;    // ±8g
    _gyroSensitivity = 32.8f;     // ±1000 dps

    // enable gyro + accel
    _modifyRegister(0x4E, 0b00101111, 0b00001111);
    _modifyRegister(0x4F, 0b11101111, 0b00100110);  // gyro 1000dps
    _modifyRegister(0x50, 0b11101111, 0b01000110);  // accel ±8g

    delay(20);
    return true;
}

float IMU::readTemperature()
{
    uint8_t buf[2];
    _spiReadBytes(0x1D, buf, 2);
    int16_t raw = (int16_t)(buf[0] << 8 | buf[1]);
    return (raw / 132.48f) + 25.0f;
}

void IMU::readAccel(float &x, float &y, float &z)
{
    uint8_t buf[6];
    _spiReadBytes(0x1F, buf, 6);
    x = ((int16_t)(buf[0] << 8 | buf[1])) / _accSensitivity;
    y = ((int16_t)(buf[2] << 8 | buf[3])) / _accSensitivity;
    z = ((int16_t)(buf[4] << 8 | buf[5])) / _accSensitivity;
}

void IMU::readGyro(float &x, float &y, float &z)
{
    uint8_t buf[6];
    _spiReadBytes(0x25, buf, 6);
    x = ((int16_t)(buf[0] << 8 | buf[1])) / _gyroSensitivity;
    y = ((int16_t)(buf[2] << 8 | buf[3])) / _gyroSensitivity;
    z = ((int16_t)(buf[4] << 8 | buf[5])) / _gyroSensitivity;
}
