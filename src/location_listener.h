#pragma once

#include "worker.h"

#include "location.h"
#include "location_list.h"

class LoRaInterface;


#define MAX_PEER_LOCATIONS  8


class LocationListener : public Worker
{
public:
    LocationListener(uint32_t station_id, LoRaInterface &lora_interface);

    const Location *get_most_recent_location() const;
    const Location *get_previous_location(const Location *location) const;

private:
    virtual void work_func();

    uint32_t _station_id;
    LoRaInterface &_lora_interface;
    LocationList _peer_locations;

    // Keep these off of the stack.
    // The code is designed to use these only in one location
    // No need to synchronize
    uint8_t _receive_buffer[LOCATION_BLOB_BYES];
    Location _location;
};