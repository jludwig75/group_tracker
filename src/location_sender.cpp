#include "location_sender.h"

#include "location_tracker.h"
#include "location_listener.h"
#include "lora_interface.h"
#include "debug_log.h"

#include <stdlib.h>


LocationSender::LocationSender(LoRaInterface &lora_interface,
                const LocationTracker &location_tracker,
                const LocationListener &location_listener,
                unsigned max_locations_to_send,
                unsigned max_time_slice_us) :
    Worker(750UL * 1000UL),
    _state(IDLE),
    _ops(lora_interface, location_tracker, location_listener, max_locations_to_send)
{
}

void LocationSender::start_sending_locations(uint32_t second)
{
    // This is called from the interrupt.
    // Don't do anything here, just transition the state.
    if (_state != IDLE)
    {
        // This is unexpected
        // TODO: This is unexpected. Log this
        DBG_LOG_ERROR("LS: unexpcted start state %u\n", _state);
    }

    _state = SEND_MY_LOCATION;
}

void LocationSender::work_func()
{
    LocationSenderOperations::OperationResult result;

    switch(_state)
    {
    case IDLE:
        break;
    case SEND_MY_LOCATION:
        DBG_LOG_INFO("LS: sending my location.\n");
        result = _ops.send_my_location();
        if (result == LocationSenderOperations::Advance)
        {
            _state = SEND_PEER_LOCATIONS;
        }
        else if (result == LocationSenderOperations::Terminate)
        {
            DBG_LOG_INFO("LS: exiting after sending my location.\n");
            _state = IDLE;
        }
        break;
    case SEND_PEER_LOCATIONS:
        DBG_LOG_INFO("LS: sending peer locations.\n");
        result = _ops.send_peer_locations();
        if (result == LocationSenderOperations::Advance ||
            result == LocationSenderOperations::Terminate)
        {
            DBG_LOG_INFO("LS: done sending locations.\n");
            _state = IDLE;
        }
        break;
    default:
        // TODO: Log this
        break;
    }
}
