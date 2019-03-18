#pragma once

#include "worker.h"

#include "location_sender.h"
#include "location_sender_ops.h"


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
    void start_sending_locations(uint32_t second);

private:
    virtual void work_func();

    enum SenderState
    {
        IDLE,
        SEND_MY_LOCATION,
        SEND_PEER_LOCATIONS
    };

    uint32_t _station_id;
    volatile SenderState _state;
    LocationSenderOperations _ops;
};