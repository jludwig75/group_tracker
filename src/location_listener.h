#pragma once

#include "worker.h"

//#include <LoRa.h>


class Location;


class LocationListener : public Worker
{
public:
    LocationListener(/*LoRaClass &lora_interface*/);

    Location *get_most_recent_location() const;
    Location *get_previous_location(Location *location) const;

private:
    virtual void work_func();
    //LoRaClass &_lora_interface;
};