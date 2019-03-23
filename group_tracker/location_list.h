#pragma once

#include "location.h"


#define MAX_LOCATIONS_TO_STORE  4


class LocationList
{
public:
    LocationList();
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
    uint8_t _insertion_index;
    LocationEntry _locations[MAX_LOCATIONS_TO_STORE];
};