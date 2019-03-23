#include "lora_interface_mock.h"


LoRaInterfaceMock::LoRaInterfaceMock(unsigned retries_before_available_to_send) :
    _retries_before_available_to_send(retries_before_available_to_send),
    _send_attempts(0)
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
    return 16; // TODO: hook this up to some configuration
}

int LoRaInterfaceMock::endPacket(bool async)
{
    return 0;
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
    return size;
}

// from Stream
int LoRaInterfaceMock::available()
{
    return 0;
}
int LoRaInterfaceMock::read()
{
    return 0;
}
int LoRaInterfaceMock::peek()
{
    return 0;
}
void LoRaInterfaceMock::flush()
{
}
