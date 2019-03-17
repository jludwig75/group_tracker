#include "location_sender.h"

#include "location_tracker.h"
#include "location_listener.h"
#include "lora_interface.h"
#include "debug_log.h"

#include <stdlib.h>


LocationSender::LocationSender(uint32_t station_id,
                LoRaInterface &lora_interface,
                const LocationTracker &location_tracker,
                const LocationListener &location_listener,
                unsigned max_locations_to_send,
                unsigned max_time_slice_us) :
    Worker(750UL * 1000UL),
    _station_id(station_id),
    _lora_interface(lora_interface),
    _location_tracker(location_tracker),
    _location_listener(location_listener),
    _max_locations_to_send(max_locations_to_send),
    _state(IDLE)
{
    
}

// TODO: The next two locations are probably a separate class.
// Maybe LocationSenderSender? :) Maybe something else.
bool LocationSender::send_location(const Location *location, bool async)
{
    uint8_t location_blob[LOCATION_BLOB_BYES];

    if (!location->pack(location_blob, sizeof(location_blob)))
    {
        return false;
    }

    // TODO: Do we add retries? Probably not to keep from taking too much time.
    // The HW probably has enough retries. Probably best to get out quickly and wait to try again.
    _lora_interface.beginPacket();
    _lora_interface.write(location_blob, LOCATION_BLOB_BYES);
    _lora_interface.endPacket(async);    // async); TODO: I don't have the latest version of the library.

    return true;
}

void LocationSender::abort_async_send()
{
    _lora_interface.terminateAsyncPacket();
}

void LocationSender::start_sending_locations(uint32_t second)
{
    // This is called from the interrupt.
    // Don't do anything here, just transition the state.
    if (_state == IDLE)
    {
        _state = SEND_LOCATIONS;
    }
    else
    {
        // This is unexpected
        // TODO: This is unexpected. Log this
    }
}

void LocationSender::work_func()
{
    // This has been broken up so we don't monopolize the CPU.
    // It is state-driven so we can keep track of what has been done.

    if (_state == SEND_LOCATIONS)
    {
        _state = IDLE;

        // Send my location
        DBG_LOG_INFO("LS: sending my location.\n");
        if (!send_my_location())
        {
            DBG_LOG_INFO("LS: exiting after sending my location.\n");
            return;
        }

        // Copy locations
        DBG_LOG_INFO("LS: copying peer locations.\n");
        if (!copy_locations_to_send())
        {
            DBG_LOG_ERROR("LS: exiting after copying peer location.\n");
            return;
        }

        // Send the peer locations
        DBG_LOG_INFO("LS: sending peer locations.\n");
        send_peer_locations();
        DBG_LOG_INFO("LS: done sending locations.\n");
    }
}

// TODO: Hmm these last three methods look like a new class
// may be LocationSenderOperations?
bool LocationSender::send_my_location()
{
    bool have_peer_locations_to_send = _location_listener.get_fist_location() != NULL;
    // We have been told to start sending our location.
    _location_tracker.get_current_location(_working_location);
    if (!send_location(&_working_location, have_peer_locations_to_send))
    {
        // TODO: should we exit or send the peer locations?
        // probably stop, because there is a HW problem
        DBG_LOG_ERROR("LS: Error sending my location\n");
        return false;
    }

    DBG_LOG_INFO("LS: have %slocations to send\n", have_peer_locations_to_send ? "" : "no ");
    return have_peer_locations_to_send;
}

bool LocationSender::copy_locations_to_send()
{
    _number_of_locations_to_send = 0;
    for(const Location *location = _location_listener.get_fist_location();
        location != NULL && _number_of_locations_to_send < _max_locations_to_send;
        location = _location_listener.get_next_location(location))
    {
        _locations_to_send[_number_of_locations_to_send] = *location;
        _number_of_locations_to_send++;
    }

    if (_number_of_locations_to_send == 0)
    {
        // TODO: This means this changed. This could happen if the policy changed in the time since the check.
        // That would mean a location/locations aged out. Otherwise it cannot change, because the listener is
        // not running.
        DBG_LOG_ERROR("LS: locations count expectedly 0\n");
        abort_async_send();
    }

    DBG_LOG_INFO("LS: %u locations to send\n", _number_of_locations_to_send);
    return _number_of_locations_to_send > 0;
}

void LocationSender::send_peer_locations()
{
    unsigned number_of_locations_sent = 0;
    DBG_LOG_INFO("LS: sending %u locations\n", _number_of_locations_to_send);
    while (number_of_locations_sent < _number_of_locations_to_send)
    {
        DBG_LOG_INFO("LS: sending location %u of %u\n", number_of_locations_sent, _number_of_locations_to_send);
        if (should_yield())
        {
            // Abort the async operation, because we're not going to get to it.
            DBG_LOG_ERROR("LS: over time\n");
            abort_async_send();
            return;
        }

        bool last_packet = _number_of_locations_to_send - number_of_locations_sent == 1;
        if (!send_location(&_locations_to_send[number_of_locations_sent], !last_packet))   // Asynch if not the last packet.
        {
            // Stop trying to send. Wait for next time to try again.
            // TODO: This may be a tune point.
            // We may need to do something here to abort the asynch send as above.
            // TODO: Not sure if this is needed after an error.
            DBG_LOG_ERROR("LS: error sending peer location\n");
            abort_async_send();
            return;
        }

        number_of_locations_sent++;
    }
    DBG_LOG_INFO("LS: sent %u locations\n", number_of_locations_sent);
}
