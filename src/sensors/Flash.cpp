#include "Flash.h"

constexpr uint16_t PAGE_SIZE = 256;
constexpr uint16_t PAGES_PER_SECTOR = 4096 / PAGE_SIZE;

Flash::Flash(uint8_t csPin, SPIClass &spiBus)
    : _csPin(csPin), _spi(&spiBus), _spiSettings(10000000, MSBFIRST, SPI_MODE0)
{
}

bool Flash::begin(uint32_t spiSpeed)
{
    pinMode(_csPin, OUTPUT);
    _deselect();
    _spiSettings = SPISettings(spiSpeed, MSBFIRST, SPI_MODE0);
    _spi->begin();

    return enter4ByteAddressingMode();
}

void Flash::_select() { digitalWrite(_csPin, LOW); }
void Flash::_deselect() { digitalWrite(_csPin, HIGH); }

// === Status registers ===
uint8_t Flash::readStatusReg1() {
    uint8_t status;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x05);
    status = _spi->transfer(0x00);
    _deselect();
    _spi->endTransaction();
    return status;
}

uint8_t Flash::readStatusReg2() {
    uint8_t status;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x35);
    status = _spi->transfer(0x00);
    _deselect();
    _spi->endTransaction();
    return status;
}

uint8_t Flash::readStatusReg3() {
    uint8_t status;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x15);
    status = _spi->transfer(0x00);
    _deselect();
    _spi->endTransaction();
    return status;
}

bool Flash::readBusyBit() { return readStatusReg1() & 0x01; }
bool Flash::readWriteEnableBit() { return readStatusReg1() & 0x02; }
bool Flash::read4ByteAddressModeBit() { return readStatusReg3() & 0x01; }

bool Flash::enter4ByteAddressingMode() {
    if (readBusyBit()) return false;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0xB7);
    _deselect();
    _spi->endTransaction();
    return read4ByteAddressModeBit();
}

bool Flash::writeEnable() {
    if (readBusyBit()) return false;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x06);
    _deselect();
    _spi->endTransaction();
    return readWriteEnableBit();
}

uint8_t Flash::readDeviceID() {
    if (readBusyBit()) return 0;
    uint8_t deviceID;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0xAB);
    _spi->transfer(0x00);
    _spi->transfer(0x00);
    _spi->transfer(0x00);
    deviceID = _spi->transfer(0x00);
    _deselect();
    _spi->endTransaction();
    return deviceID;
}

bool Flash::works() { return readDeviceID() == 0x18; }

// === Memory operations ===
bool Flash::readMemory(uint32_t address, uint16_t length, uint8_t* buffer) {
    if (readBusyBit()) return false;
    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x13);
    _spi->transfer((address >> 24) & 0xFF);
    _spi->transfer((address >> 16) & 0xFF);
    _spi->transfer((address >> 8) & 0xFF);
    _spi->transfer(address & 0xFF);
    for (uint16_t i = 0; i < length; i++)
        buffer[i] = _spi->transfer(0x00);
    _deselect();
    _spi->endTransaction();
    return true;
}

bool Flash::readPage(uint32_t pageNumber, uint8_t* buffer) {
    return readMemory(pageNumber * PAGE_SIZE, PAGE_SIZE, buffer);
}

bool Flash::writePage(uint32_t pageNumber, const uint8_t* buffer) {
    uint32_t address = pageNumber * PAGE_SIZE;
    if (readBusyBit() || !readWriteEnableBit()) return false;

    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x12);
    _spi->transfer((address >> 24) & 0xFF);
    _spi->transfer((address >> 16) & 0xFF);
    _spi->transfer((address >> 8) & 0xFF);
    _spi->transfer(address & 0xFF);
    for (uint16_t i = 0; i < PAGE_SIZE; i++)
        _spi->transfer(buffer[i]);
    _deselect();
    _spi->endTransaction();
    return true;
}

uint32_t Flash::sectorNumberFromPage(uint32_t pageNumber) {
    return pageNumber / PAGES_PER_SECTOR;
}

bool Flash::sectorErase(uint32_t sectorNumber) {
    uint32_t address = sectorNumber * 4096;
    if (readBusyBit() || !readWriteEnableBit()) return false;

    _spi->beginTransaction(_spiSettings);
    _select();
    _spi->transfer(0x21);
    _spi->transfer((address >> 24) & 0xFF);
    _spi->transfer((address >> 16) & 0xFF);
    _spi->transfer((address >> 8) & 0xFF);
    _spi->transfer(address & 0xFF);
    _deselect();
    _spi->endTransaction();
    return true;
}

bool Flash::eraseUpToPage(uint32_t pageNumber) {
    if (readBusyBit() || !readWriteEnableBit()) return false;
    uint32_t lastSector = sectorNumberFromPage(pageNumber);
    for (uint32_t s = 0; s <= lastSector; s++)
        if (!sectorErase(s)) return false;
    return true;
}
