#pragma once

#include "location.h"


#define MAX_LOCATIONS_TO_STORE  8


class LocationList
{
public:
    void store_location(const Location &location);

    const Location *get_most_recent_location() const;
    const Location *get_previous_location(const Location *location) const;

private:
    Location _locations[MAX_LOCATIONS_TO_STORE];
};