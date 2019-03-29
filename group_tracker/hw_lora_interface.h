#pragma once

#include "lora_interface.h"


class LoRaClass;

class HwLoRaInterface : public LoRaInterface
{
public:
    HwLoRaInterface(LoRaClass & lora);
    virtual int beginPacket(int implicitHeader = false);
    virtual int endPacket(bool async = false);

    virtual int parsePacket(int size = 0);
    virtual int packetRssi();
    virtual float packetSnr();
    virtual long packetFrequencyError();

    virtual void receive();
    virtual void idle();

    // from Print
    virtual unsigned write(uint8_t byte);
    virtual unsigned write(const uint8_t *buffer, unsigned size);

    // from Stream
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush();
private:
    LoRaClass & _lora;
};
