#pragma once

#include "worker.h"

#include "location.h"
#include "location_list.h"

class LoRaInterface;


#define MAX_PEER_LOCATIONS  8


class LocationListener : public Worker
{
public:
    LocationListener(LoRaInterface &lora_interface);

    const Location *get_most_recent_location() const;
    const Location *get_previous_location(const Location *location) const;

private:
    virtual void work_func();

    LoRaInterface &_lora_interface;
    LocationList _peer_locations;
};