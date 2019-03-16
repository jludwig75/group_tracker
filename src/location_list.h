#pragma once

#include "location.h"


#define MAX_LOCATIONS_TO_STORE  8


class LocationList
{
public:
    LocationList(unsigned locations_to_store);
    void store_location(int rssi, float snr, const Location &location);

    const Location *get_fist_location() const;
    const Location *get_next_location(const Location *location) const;

private:
    struct LocationEntry
    {
        bool used;
        int rssi;
        float snr;
        Location location;
    };
    const unsigned _locations_to_store;
    unsigned _insertion_index;
    LocationEntry _locations[MAX_LOCATIONS_TO_STORE];
};