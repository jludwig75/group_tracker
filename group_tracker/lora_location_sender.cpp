#include "lora_location_sender.h"

#include "lora_interface.h"
#include "location.h"

LoRaLocationSender::LoRaLocationSender(LoRaInterface & lora_interface) :
    _lora_interface(lora_interface)
{
}

LoRaLocationSender::SendLocationResult LoRaLocationSender::send_location(const Location *location)
{
    uint8_t location_blob[LOCATION_BLOB_BYES];

    if (!location->pack(location_blob, sizeof(location_blob)))
    {
        return Error;
    }

    // See if the interface is ready to send yet.
    if (_lora_interface.beginPacket() == 0)
    {
        return Retry;
    }

    _lora_interface.write(location_blob, LOCATION_BLOB_BYES);
    // TODO: Check for errors
    _lora_interface.endPacket(true);    // async); TODO: I don't have the latest version of the library.
    // TODO: Check for errors

    return Success;
}
