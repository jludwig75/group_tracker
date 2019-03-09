#pragma once

#include "worker.h"

#include "location.h"

#include <Arduino.h>


class LocationTracker : public Worker
{
public:
    LocationTracker(Serial_ & gps_serial_interface);

    const Location &get_current_location() const;

private:
    virtual void work_func();

    Serial_ & gps_serial_interface;
    Location _current_location;
};