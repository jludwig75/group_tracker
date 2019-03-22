#pragma once

#include "worker.h"

#include "location.h"
#include "my_Adafruit_GPS.h"

class SoftwareSerial;
class GpsClock;

class LocationTracker : public Worker
{
public:
    LocationTracker(uint32_t station_id, SoftwareSerial *gps_serial_interface, GpsClock &gps_clock);

    void begin();

    void get_current_location(Location &location) const;

    void set_dummy_location(float latitude, float longitude);

private:
    virtual void do_work();

    uint32_t _station_id;
    SoftwareSerial *_gps_serial_interface;
    Adafruit_GPS _gps;
    GpsClock &_gps_clock;
    Location _current_location;
};
