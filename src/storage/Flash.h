#ifndef FLASH_H
#define FLASH_H

#include <Arduino.h>
#include <SPI.h>

class Flash {
public:
    Flash(uint8_t csPin = 17, SPIClass &spiBus = SPI);

    bool begin(uint32_t spiSpeed = 10000000); // sets up CS pin and SPI, enters 4-byte addressing mode

    // Status / info
    uint8_t readStatusReg1();
    uint8_t readStatusReg2();
    uint8_t readStatusReg3();
    bool readBusyBit();
    bool readWriteEnableBit();
    bool read4ByteAddressModeBit();
    bool enter4ByteAddressingMode();
    bool writeEnable();
    uint8_t readDeviceID();
    bool works();

    // Memory operations
    bool readMemory(uint32_t address, uint16_t length, uint8_t* buffer);
    bool readPage(uint32_t pageNumber, uint8_t* buffer);
    bool writePage(uint32_t pageNumber, const uint8_t* buffer);
    uint32_t sectorNumberFromPage(uint32_t pageNumber);
    bool sectorErase(uint32_t sectorNumber);
    bool eraseUpToPage(uint32_t pageNumber);

private:
    uint8_t _csPin;
    SPIClass* _spi;
    SPISettings _spiSettings;

    void _select();
    void _deselect();
};

#endif
