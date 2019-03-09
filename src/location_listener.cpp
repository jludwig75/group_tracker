#include "location_listener.h"

#include "lora_interface.h"



LocationListener::LocationListener(LoRaInterface &lora_interface) :
    Worker(1000),
    _lora_interface(lora_interface),
    _peer_locations()
{

}


void LocationListener::work_func()
{
    uint8_t receive_buffer[LOCATION_BLOB_BYES];

    /*
     * TODO: Learn about errors and what to do with these functions.
     * Also need to configure HW to do as much error checking and correction as it can.
     * packetRssi()
     * packetSnr()
     * packetFrequencyError()
     */


    unsigned packetSize = _lora_interface.parsePacket();
    if (packetSize >= sizeof(receive_buffer)) {
        if (packetSize > sizeof(receive_buffer))
        {
            // TODO: log this. Need to know if this can happen.
        }

        // read packet
        unsigned i = 0;
        while (_lora_interface.available() && i < sizeof(receive_buffer)) {
            int byte_read = _lora_interface.read();
            if (byte_read < 0 || byte_read > UINT8_MAX)
            {
                // TODO: Log this
                // Exit or continue reading. The software only returns an error here
                // if available returned 0 bytes. Shouldn't happen. Either way we'll exit.
                return;
            }
            receive_buffer[i] = (uint8_t)byte_read;
            i++;
        }

        if (i != sizeof(receive_buffer))
        {
            // TODO: Log this
            return;
        }

        // print RSSI of packet
        //Serial.print("' with RSSI ");
        //Serial.println(_lora_interface.packetRssi());

        Location location;
        if (!location.un_pack(receive_buffer, sizeof(receive_buffer)))
        {
            // TODO: Log this
            return;
        }

        _peer_locations.store_location(location);
    }    
}

const Location *LocationListener::get_most_recent_location() const
{
    return _peer_locations.get_most_recent_location();
}

const Location *LocationListener::get_previous_location(const Location *location) const
{
    return _peer_locations.get_previous_location(location);
}
