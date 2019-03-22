#pragma once

#include "location_tracker.h"
#include "location_listener.h"
#include "location_sender.h"
#include "gps_clock.h"


class Stream;
class Worker;
class LoraInterface;


#define NUM_WORKERS 3


class GroupLocator
{
public:
    GroupLocator(uint32_t station_id,
                 unsigned stations_per_group,
                 SoftwareSerial *gps_serial_interface,
                 LoRaInterface &lora_interface,
                 unsigned transmission_time_sec,
                 unsigned max_peer_locations_to_store,
                 unsigned max_peer_locations_to_send);

    void begin();

    void on_pps_interrupt();

    void on_loop();

    void set_dummy_location(float latitude, float longitude);
    void set_dummy_time(time_t time);

private:
    uint32_t get_active_station_number(uint32_t second) const;

    uint32_t _station_id;
    uint8_t _stations_per_group;
    uint8_t _active_worker;
    bool _use_timer_for_gps;
    unsigned _transmission_time_sec;
    bool _enable_communication;
    GpsClock _gps_clock;
    LocationTracker _location_tracker;
    LocationListener _location_listener;
    LocationSender _location_sender;
    Worker *_workers[NUM_WORKERS];
};