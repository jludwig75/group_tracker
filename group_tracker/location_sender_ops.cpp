#include "location_sender_ops.h"

#include "location_listener.h"
#include "location_tracker.h"
#include "debug_log.h"


LocationSenderOperations::LocationSenderOperations(LoRaInterface & lora_interface,
                                                   const LocationTracker &location_tracker,
                                                   const LocationListener &location_listener,
                                                   unsigned max_locations_to_send) :
    _lora(lora_interface),
    _location_tracker(location_tracker),
    _location_listener(location_listener),
    _max_locations_to_send(max_locations_to_send),
    _locations_to_send(),
    _number_of_locations_to_send(0),
    _number_of_locations_sent(0)
{
}

LocationSenderOperations::OperationResult LocationSenderOperations::send_my_location()
{
    _number_of_locations_to_send = 0;
    _number_of_locations_sent = 0;

    _location_tracker.get_current_location(_working_location);

    LoRaLocationSender::SendLocationResult result = _lora.send_location(&_working_location);
    if (result == LoRaLocationSender::Retry)
    {
        return Retry;
    }
    if (result == LoRaLocationSender::Error)
    {
        return Terminate;
    }

    copy_locations_to_send();
    if (_number_of_locations_to_send == 0)
    {
        return Terminate;
    }

    return Advance;
}

void LocationSenderOperations::copy_locations_to_send()
{
    _number_of_locations_to_send = 0;
    _number_of_locations_sent = 0;

    for(const Location *location = _location_listener.get_fist_location();
        location != NULL && _number_of_locations_to_send < _max_locations_to_send;
        location = _location_listener.get_next_location(location))
    {
        _locations_to_send[_number_of_locations_to_send] = *location;
        _number_of_locations_to_send++;
    }

    DBG_LOG_INFO("LSO: %u locations to send\n", _number_of_locations_to_send);
}

LocationSenderOperations::OperationResult LocationSenderOperations::send_peer_locations()
{
    if (_number_of_locations_sent >= _number_of_locations_to_send)
    {
        return Advance;
    }

    DBG_LOG_INFO("LS: sending location %u of %u\n", _number_of_locations_sent, _number_of_locations_to_send);

    LoRaLocationSender::SendLocationResult result = _lora.send_location(&_locations_to_send[_number_of_locations_sent]);
    if (result == LoRaLocationSender::Retry)
    {
        return Retry;
    }
    if (result == LoRaLocationSender::Error)
    {
        return Terminate;
    }

    // Advance to the next location
    _number_of_locations_sent++;
    if (_number_of_locations_sent < _number_of_locations_to_send)
    {
        // More locations to send.
        return Retry;
    }

    DBG_LOG_INFO("LS: sent %u locations\n", _number_of_locations_sent);
    return Advance;
}
