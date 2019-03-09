#pragma once

#include <stdint.h>


class LoRaInterface
{
public:
    virtual ~LoRaInterface() {}
    virtual int beginPacket(int implicitHeader = false) = 0;
    virtual int endPacket() = 0;

    virtual int parsePacket(int size = 0) = 0;
    virtual int packetRssi() = 0;
    virtual float packetSnr() = 0;
    virtual long packetFrequencyError() = 0;

    // from Print
    virtual unsigned write(uint8_t byte) = 0;
    virtual unsigned write(const uint8_t *buffer, unsigned size) = 0;

    // from Stream
    virtual int available() = 0;
    virtual int read() = 0;
    virtual int peek() = 0;
    virtual void flush() = 0;
};