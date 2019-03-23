#include "hw_lora_interface.h"


#include <LoRa.h>


HwLoRaInterface::HwLoRaInterface(LoRaClass & lora) :
    _lora(lora)
{
}

int HwLoRaInterface::beginPacket(int implicitHeader)
{
    return _lora.beginPacket(implicitHeader);
}
int HwLoRaInterface::endPacket(bool async)
{
    return _lora.endPacket(async);
}

int HwLoRaInterface::parsePacket(int size)
{
    return _lora.parsePacket(size);
}
int HwLoRaInterface::packetRssi()
{
    return _lora.packetRssi();
}
float HwLoRaInterface::packetSnr()
{
    return _lora.packetSnr();
}
long HwLoRaInterface::packetFrequencyError()
{
    return _lora.packetFrequencyError();
}

// from Print
unsigned HwLoRaInterface::write(uint8_t byte)
{
    return _lora.write(byte);
}
unsigned HwLoRaInterface::write(const uint8_t *buffer, unsigned size)
{
    return _lora.write(buffer, size);
}

// from Stream
int HwLoRaInterface::available()
{
    return _lora.available();
}
int HwLoRaInterface::read()
{
    return _lora.read();
}
int HwLoRaInterface::peek()
{
    return _lora.peek();
}
void HwLoRaInterface::flush()
{
    _lora.flush();
}
