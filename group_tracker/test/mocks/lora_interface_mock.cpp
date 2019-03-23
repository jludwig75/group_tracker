#include "lora_interface_mock.h"


LoRaInterfaceMock::LoRaInterfaceMock(unsigned retries_before_available_to_send,
                        bool fail_write,
                        bool fail_end_packet,
                        bool fail_read) :
    _retries_before_available_to_send(retries_before_available_to_send),
    _send_attempts(0),
    _fail_write(fail_write),
    _fail_end_packet(fail_end_packet),
    _fail_read(fail_read),
    _num_bytes_written(0),
    _num_bytes_read(0)
{

}

int LoRaInterfaceMock::beginPacket(int implicitHeader)
{
    if (_send_attempts < _retries_before_available_to_send)
    {
        _send_attempts--;
        return 0;
    }
    
    _send_attempts--;
    return 1;
}

int LoRaInterfaceMock::endPacket(bool async)
{
    return _fail_end_packet ? 0 : 1;
}

int LoRaInterfaceMock::parsePacket(int size)
{
    return 0;
}
int LoRaInterfaceMock::packetRssi()
{
    return 0;
}
float LoRaInterfaceMock::packetSnr()
{
    return 0;
}
long LoRaInterfaceMock::packetFrequencyError()
{
    return 0;
}

// from Print
unsigned LoRaInterfaceMock::write(uint8_t byte)
{
    return 1;
}
unsigned LoRaInterfaceMock::write(const uint8_t *buffer, unsigned size)
{
    if (_fail_write)
    {
        return 0;
    }

    _num_bytes_written += size;
    return size;
}

// from Stream
int LoRaInterfaceMock::available()
{
    return 0;
}
int LoRaInterfaceMock::read()
{
    // TODO: Do somethin here like pre-loading bytes to read.
    if (_fail_read)
    {
        return 0;
    }

    _num_bytes_read += 0;
    return 0;
}
int LoRaInterfaceMock::peek()
{
    return 0;
}
void LoRaInterfaceMock::flush()
{
}

unsigned LoRaInterfaceMock::get_num_bytes_written() const
{
    return _num_bytes_written;
}

unsigned LoRaInterfaceMock::get_num_bytes_read() const
{
    return _num_bytes_read;
}
