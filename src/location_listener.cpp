#include "location_listener.h"

#include "lora_interface.h"



LocationListener::LocationListener(uint32_t station_id, LoRaInterface &lora_interface, unsigned max_peer_locations_to_store) :
    Worker(),
    _station_id(station_id),
    _lora_interface(lora_interface),
    _peer_locations(max_peer_locations_to_store)
{
}


void LocationListener::work_func()
{

    /*
     * TODO: Learn about errors and what to do with these functions.
     * Also need to configure HW to do as much error checking and correction as it can.
     * packetRssi()
     * packetSnr()
     * packetFrequencyError()
     */


    unsigned packetSize = _lora_interface.parsePacket();
    if (packetSize >= sizeof(_receive_buffer)) {
        if (packetSize > sizeof(_receive_buffer))
        {
            // TODO: log this. Need to know if this can happen.
        }

        // read packet
        unsigned i = 0;
        while (_lora_interface.available() && i < sizeof(_receive_buffer)) {
            int byte_read = _lora_interface.read();
            if (byte_read < 0 || byte_read > UINT8_MAX)
            {
                // TODO: Log this
                // Exit or continue reading. The software only returns an error here
                // if available returned 0 bytes. Shouldn't happen. Either way we'll exit.
                return;
            }
            _receive_buffer[i] = (uint8_t)byte_read;
            i++;
        }

        if (i != sizeof(_receive_buffer))
        {
            // TODO: Log this
            return;
        }

        if (!_location.un_pack(_receive_buffer, sizeof(_receive_buffer)))
        {
            // TODO: Log this
            return;
        }

        if (_location.get_station_id() != _station_id)
        {
            // Don't store our own location as a peer location.
            _peer_locations.store_location(_lora_interface.packetRssi(), _lora_interface.packetSnr(), _location);
        }
    }    
}

const Location *LocationListener::get_fist_location() const
{
    return _peer_locations.get_fist_location();
}

const Location *LocationListener::get_next_location(const Location *location) const
{
    return _peer_locations.get_next_location(location);
}
