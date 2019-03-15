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
    GroupLocator(uint32_t station_id, SoftwareSerial *gps_serial_interface, LoRaInterface &lora_interface);

    void begin();

    void on_pps_interrupt();

    void on_loop();

private:
    GpsClock _gps_clock;
    LocationTracker _location_tracker;
    LocationListener _location_listener;
    LocationSender _location_sender;
    Worker *_workers[NUM_WORKERS];
};