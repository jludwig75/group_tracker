#pragma once

#include "worker.h"

#include "location.h"

class Stream;

class LocationTracker : public Worker
{
public:
    LocationTracker(Stream & gps_serial_interface);

    const Location &get_current_location() const;

private:
    virtual void work_func();

    Stream & gps_serial_interface;
    Location _current_location;
};