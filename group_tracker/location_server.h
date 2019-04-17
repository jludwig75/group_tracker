#pragma once


#include <WString.h>


class LocationList;


class LocationServer
{
public:
    LocationServer(const LocationList &location_list);
    void begin();
    void on_loop();
    String handle_list_locations() const;
private:
    const LocationList & _location_list;
};
