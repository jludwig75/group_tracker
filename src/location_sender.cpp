#include "location_sender.h"

#include "location_tracker.h"
#include "location_listener.h"
#include "lora_interface.h"

#include <stdlib.h>


LocationSender::LocationSender(uint32_t station_id,
                LoRaInterface &lora_interface,
                const LocationTracker &location_tracker,
                const LocationListener &location_listener,
                unsigned max_locations_to_send,
                unsigned max_time_slice_us) :
    Worker(max_time_slice_us),
    _station_id(station_id),
    _lora_interface(lora_interface),
    _location_tracker(location_tracker),
    _location_listener(location_listener),
    _max_locations_to_send(max_locations_to_send),
    _state(IDLE)
{
    
}

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
    _lora_interface.endPacket();    // async); TODO: I don't have the latest version of the library.

    return true;
}

void LocationSender::send_locations(uint32_t second)
{
    // This is called from the interrupt.
    // Don't do anything here, just transition the state.
    if (_state == IDLE)
    {
        _state = SEND_MY_LOCATION;
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

    switch(_state)
    {
        case IDLE:
            // We have not been told to do anything yet.
            break;
        
        case SEND_MY_LOCATION:
            {
                bool have_peer_locations_to_send = _location_listener.get_most_recent_location() != NULL;
                // We have been told to start sending our location.
                if (!send_location(&_location_tracker.get_current_location()), have_peer_locations_to_send)
                {
                    // TODO: log this
                    // TODO: should we exit or send the peer locations?
                    // probably stop, because there is a HW problem
                    _state = IDLE;
                    return;
                }
                if (!have_peer_locations_to_send)
                {
                    _state = IDLE;
                    break;
                }
                _state = COPY_PEER_LOCATIONS;
                // See if we've exceeded out time slice
                if (should_yield())
                {
                    break;
                }
            }
        case COPY_PEER_LOCATIONS:
            // Copy locations
            if (copy_locations_to_send() == 0)
            {
                // TODO: This means this changed.
                // TODO: We need to add an abort asynch operation that can wait for the send to complete and clear IRQ's
                // Probably add a method to LoRaClass like wait_for_asynch_packet()
                // Or resend our location to complete the operation. Seems heavy wait and like a hack though.
                _state = IDLE;
                break;
            }
            _number_of_locations_sent = 0;
            _state = SEND_PEER_LOCATIONS;
            // No need to yield here. COPY_PEER_LOCATIONS doesn't take that long.
        case SEND_PEER_LOCATIONS:
            {
                // TODO: There is also a bigger window (i.e. 50% of each sender's time window) that we cannot exceed.
                while (_number_of_locations_sent < _number_of_locations_to_send)
                {
                    if (should_yield())
                    {
                        break;
                    }

                    // TODO: Is there a limit to how much data we can send asyncrhonously?
                    // I doubt it, because we are sending so little. If so, this may be what
                    // limits how many peer locations we can send.
                    bool last_packet = _number_of_locations_to_send - _number_of_locations_sent == 1;
                    if (!send_location(&_locations_to_send[_number_of_locations_sent], !last_packet))   // Asynch if not the last packet.
                    {
                        // Stop trying to send. Wait for next time to try again.
                        // TODO: This may be a tune point.
                        // We may need to do something here to abort the asynch send as above.
                        _state = IDLE;
                        break;
                    }
                    _number_of_locations_sent++;
                }
                // We're done.
                _state = IDLE;
            }
    }
}

unsigned LocationSender::copy_locations_to_send()
{
    _number_of_locations_to_send = 0;
    for(const Location *location = _location_listener.get_most_recent_location();
        location != NULL && _number_of_locations_to_send < _max_locations_to_send;
        location = _location_listener.get_previous_location(location))
    {
        _locations_to_send[_number_of_locations_to_send] = *location;
        _number_of_locations_to_send++;
    }

    return _number_of_locations_to_send;
}
