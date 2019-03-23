#pragma once

#include "lora_interface.h"


class LoRaInterfaceMock : public LoRaInterface
{
public:
    LoRaInterfaceMock(unsigned retries_before_available_to_send);
    virtual int beginPacket(int implicitHeader = false);
    virtual int endPacket(bool async = false);

    virtual int parsePacket(int size = 0);
    virtual int packetRssi();
    virtual float packetSnr();
    virtual long packetFrequencyError();

    // from Print
    virtual unsigned write(uint8_t byte);
    virtual unsigned write(const uint8_t *buffer, unsigned size);

    // from Stream
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush();
private:
    bool _retries_before_available_to_send;
    unsigned _send_attempts;
};