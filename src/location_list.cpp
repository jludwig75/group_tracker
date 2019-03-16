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

LocationList::LocationList(unsigned locations_to_store) :
    _locations_to_store(locations_to_store),
    _insertion_index(0)
{
    // assert locations_to_store <= MAX_LOCATIONS_TO_STORE

    for (unsigned i = 0; i < MAX_LOCATIONS_TO_STORE; i++)
    {
        _locations[i].used = false;
    }
}

void LocationList::store_location(int rssi, float snr, const Location &location)
{
    _locations[_insertion_index].used = true;
    _locations[_insertion_index].rssi = rssi;
    _locations[_insertion_index].snr = snr;
    _locations[_insertion_index].location = location;

    _insertion_index = (_insertion_index + 1) % _locations_to_store;
}

// These handle fragmentation, but not starting at any location but the beginning.
const Location *LocationList::get_fist_location() const
{
    for(unsigned i = 0; i < _locations_to_store; i++)
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
    if (location < &_locations[0].location || location >= &_locations[_locations_to_store].location)
    {
        return NULL;
    }

    // We don't do wrapping yet, so this is easier. Otherwise
    // we need to keep track of the index we started enumerating
    // at, if we do ever start in the middle, because of how implement
    // location priority.
    unsigned i = (location - &_locations[0].location) + 1;
    for(; i < _locations_to_store; i++)
    {
        if (_locations[i].used)
        {
            return &_locations[i].location;
        }
    }
    return NULL;
}

