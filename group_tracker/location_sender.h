#pragma once

#include "worker.h"

#include "location_sender.h"
#include "location_sender_ops.h"


class LocationTracker;
class LocationListener;
class LoRaInterface;


class LocationSender : public Worker
{
public:
    LocationSender(LoRaInterface &lora_interface,
                    const LocationTracker &location_tracker,
                    const LocationListener &location_listener);
    
    // Callable from interrupt
    void start_sending_locations(uint32_t second);

private:
    virtual void do_work();

    enum SenderState
    {
        IDLE,
        SEND_MY_LOCATION,
        SEND_PEER_LOCATIONS
    };

    volatile SenderState _state;
    LocationSenderOperations _ops;
};