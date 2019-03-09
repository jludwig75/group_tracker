#pragma once

#include "location_tracker.h"
#include "location_listener.h"
#include "location_sender.h"


class Stream;
class Worker;
class LoraInterface;


#define NUM_WORKERS 3


class GroupLocator
{
public:
    GroupLocator(Stream & gps_serial_interface, LoRaInterface &lora_interface);

    bool begin();

    void on_pps_interrupt();

    void on_loop();

private:
    LocationTracker _location_tracker;
    LocationListener _location_listener;
    LocationSender _location_sender;
    Worker *_workers[NUM_WORKERS];
};