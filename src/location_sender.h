#pragma once

#include "worker.h"

#include "location.h"


class LocationTracker;
class LocationListener;
class LoRaInterface;


#define MAX_LOCATIONS_TO_SEND   8


class LocationSender : public Worker
{
public:
    LocationSender(uint32_t station_id,
                    LoRaInterface &lora_interface,
                    const LocationTracker &location_tracker,
                    const LocationListener &location_listener,
                    unsigned max_locations_to_send,
                    unsigned max_time_slice_us);
    
    // Callable from interrupt
    void send_locations(uint32_t second);

private:
    virtual void work_func();

    enum SenderState
    {
        IDLE,
        SEND_MY_LOCATION,
        COPY_PEER_LOCATIONS,
        SEND_PEER_LOCATIONS
    };

    bool send_location(const Location *location, bool async = true);

    unsigned copy_locations_to_send();
    
    uint32_t _station_id;
    LoRaInterface &_lora_interface;
    const LocationTracker &_location_tracker;
    const LocationListener &_location_listener;
    const unsigned _max_locations_to_send;
    volatile SenderState _state;
    Location _locations_to_send[MAX_LOCATIONS_TO_SEND];
    unsigned _number_of_locations_to_send;
    unsigned _number_of_locations_sent;
};