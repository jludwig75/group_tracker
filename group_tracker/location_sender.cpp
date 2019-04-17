#include "location_sender.h"

#include "location_tracker.h"
#include "location_listener.h"
#include "lora_interface.h"

#define DBG_LOG_LEVEL   DBG_LOG_LEVEL_INFO
#include "debug_log.h"

#include <stdlib.h>


LocationSender::LocationSender(LoRaInterface &lora_interface,
                const LocationTracker &location_tracker,
                const LocationListener &location_listener) :
    _state(IDLE),
    _ops(lora_interface, location_tracker, location_listener)
{
}

void LocationSender::start_sending_locations(uint32_t second)
{
    // This is called from the interrupt.
    // Don't do anything here, just transition the state.
    if (_state != IDLE)
    {
        DBG_LOG_ERROR("LS: unexpcted state %u\n", _state);
    }

    _state = SEND_MY_LOCATION;
}

void LocationSender::do_work()
{
    LocationSenderOperations::OperationResult result;

    switch(_state)
    {
    case IDLE:
        break;
    case SEND_MY_LOCATION:
        DBG_LOG_INFO("LS: sending my loc\n");
        result = _ops.send_my_location();
        if (result == LocationSenderOperations::Advance)
        {
            _state = SEND_PEER_LOCATIONS;
        }
        else if (result == LocationSenderOperations::Terminate)
        {
            DBG_LOG_INFO("LS: exit after sending my loc\n");
            _state = IDLE;
        }
        break;
    case SEND_PEER_LOCATIONS:
        DBG_LOG_DEBUG("LS: sending peer locs\n");
        result = _ops.send_peer_locations();
        if (result == LocationSenderOperations::Advance ||
            result == LocationSenderOperations::Terminate)
        {
            DBG_LOG_INFO("LS: done sending locs\n");
            _state = IDLE;
        }
        break;
    default:
        // TODO: Log this
        break;
    }
}
