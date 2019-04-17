#include "location_list.h"

#include <stdlib.h>

/*
 * This class controls the policy of what locations to store
 * and the order they are returned.
 * 
 * We want to store the most relevant locations for other stations
 * 
 * We want to sort these by importance so we enumerate them to callers
 * in relevancy order.
 * 
 * The goal is to prioritize the locations by relevancy to other stations.
 * If we are closer to some stations (rssi is high), we may want to broadcast
 * those locations to other stations that are not as close. However, if there
 * is a location we barely pick up, it may be more relevant to other stations
 * than locations we pick up strongly. It seems like a catch 22. I think we
 * should split the list in two. One half can keep track of the weakest, valid
 * locations and the other half can keep track of the strongest locations. Or
 * may be the weakst go in odd indexs and the strongest in even to keep them
 * sorted so we alternate strong and weak when enumerating locations to callers.
 * 
 * This could get really complex. We need to keep it simple.
 * 
 * For now it is just a circular list. This is just to get it up and running.
 * We will also want to age out old locations too, which will make the list
 * fragmented.
 */

LocationList::LocationList() :
    _insertion_index(0)
{
    // assert locations_to_store <= MAX_LOCATIONS_TO_STORE

    for (uint8_t i = 0; i < MAX_LOCATIONS_TO_STORE; i++)
    {
        _locations[i].used = false;
    }
}

void LocationList::store_location(int rssi, float snr, const Location &location)
{
    // TODO: First see if we've already stored a location for this station and replace it. 
    for (uint8_t i = 0; i < MAX_LOCATIONS_TO_STORE; i++)
    {
        if (_locations[i].used && _locations[i].location.get_station_id() == location.get_station_id())
        {
            _locations[i].used = true;
            _locations[i].rssi = rssi;
            _locations[i].snr = snr;
            _locations[i].location = location;

            if (_insertion_index == i)
            {
                // Advance the insertion index so the next location stored
                // Doesn't overwrite this one.
                // TODO: It may get overwtiten later. May be other ways to handle this.
                _insertion_index = (_insertion_index + 1) % MAX_LOCATIONS_TO_STORE;
            }
            return;
        }
    }

    _locations[_insertion_index].used = true;
    _locations[_insertion_index].rssi = rssi;
    _locations[_insertion_index].snr = snr;
    _locations[_insertion_index].location = location;

    _insertion_index = (_insertion_index + 1) % MAX_LOCATIONS_TO_STORE;
}

// These handle fragmentation, but not starting at any location but the beginning.
const Location *LocationList::get_fist_location() const
{
    for(uint8_t i = 0; i < MAX_LOCATIONS_TO_STORE; i++)
    {
        if (_locations[i].used)
        {
            return &_locations[i].location;
        }
    }
    return NULL;
}

// These handle fragmentation, but not starting at any location but the beginning.
const Location *LocationList::get_next_location(const Location *location) const
{
    if (location < &_locations[0].location || location >= &_locations[MAX_LOCATIONS_TO_STORE].location)
    {
        return NULL;
    }

    // We don't do wrapping yet, so this is easier. Otherwise
    // we need to keep track of the index we started enumerating
    // at, if we do ever start in the middle, because of how implement
    // location priority.
    uint8_t i = (location - &_locations[0].location) + 1;
    for(; i < MAX_LOCATIONS_TO_STORE; i++)
    {
        if (_locations[i].used)
        {
            return &_locations[i].location;
        }
    }
    return NULL;
}
