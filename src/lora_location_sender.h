#pragma once


class LoRaInterface;
class Location;


class LoRaLocationSender
{
public:
    enum SendLocationResult { Success, Retry, Error };

    LoRaLocationSender(LoRaInterface & lora_interface);

    SendLocationResult send_location(const Location *location);

private:
    LoRaInterface & _lora_interface;
};