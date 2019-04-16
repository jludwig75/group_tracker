#pragma once


class LocationList;


class LocationServer
{
public:
    LocationServer(const LocationList &location_list);
    void begin();
    void on_loop();
private:
    const LocationList &_location_list;
};
