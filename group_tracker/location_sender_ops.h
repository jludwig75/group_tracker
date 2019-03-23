#pragma once

#include "location.h"
#include "lora_location_sender.h"

class LoRaInterface;
class LocationListener;
class LocationTracker;


#define MAX_LOCATIONS_TO_SEND   4


class LocationSenderOperations
{
public:
    enum OperationResult { Terminate, Retry, Advance };

    LocationSenderOperations(LoRaInterface & lora_interface,
                    const LocationTracker &location_tracker,
                    const LocationListener &location_listener);

    OperationResult send_my_location();
    OperationResult send_peer_locations();

private:
    void copy_locations_to_send();

    LoRaLocationSender _lora;
    const LocationTracker &_location_tracker;
    const LocationListener &_location_listener;
    Location _locations_to_send[MAX_LOCATIONS_TO_SEND];
    uint8_t _number_of_locations_to_send;
    uint8_t _number_of_locations_sent;

    // Keep this off of the stack
    Location _working_location;
};