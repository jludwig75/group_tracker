#pragma once

#include "lora_interface.h"


class LoRaInterfaceMock : public LoRaInterface
{
public:
    LoRaInterfaceMock(unsigned retries_before_available_to_send = 0,
                        bool fail_write = false,
                        bool fail_end_packet = false,
                        bool fail_read = false);
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

    unsigned get_num_bytes_written() const;
    unsigned get_num_bytes_read() const;
    
private:
    bool _retries_before_available_to_send;
    unsigned _send_attempts;
    bool _fail_write;
    bool _fail_end_packet;
    bool _fail_read;
    unsigned _num_bytes_written;
    unsigned _num_bytes_read;
};