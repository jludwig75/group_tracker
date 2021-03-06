#include "location_listener.h"

#include "lora_interface.h"

#define DBG_LOG_LEVEL   DBG_LOG_LEVEL_INFO
#include "debug_log.h"

#if DBG_LOG_LEVEL >= DBG_LOG_LEVEL_INFO
#include <Arduino.h>
#endif // DBG_LOG_LEVEL >= DBG_LOG_LEVEL_INFO



LocationListener::LocationListener(uint32_t station_id, LoRaInterface &lora_interface) :
    _station_id(station_id),
    _lora_interface(lora_interface),
    _peer_locations()
{
}

const LocationList & LocationListener::get_location_list() const
{
    return _peer_locations;
}


static void discard_excess_rx_data(LoRaInterface & lora_interface, unsigned bytes_to_clear)
{
    uint8_t i = 0;

    DBG_LOG_ERROR("LL: clearing %u excess bytes\n", bytes_to_clear);

    while (lora_interface.available()) {
        int byte_read = lora_interface.read();
        if (byte_read < 0 || byte_read > UINT8_MAX)
        {
            // Give up trying to read this and exit.
            DBG_LOG_ERROR("LL: failed to read byte\n");
            break;
        }
        i++;
    }

    DBG_LOG_ERROR("LL: cleared %u excess bytes\n", i);
}


void LocationListener::on_receive(int packet_size)
{
    DBG_LOG_INFO("LL: recevied %u bytes\n", packet_size);
    if (packet_size < sizeof(_receive_buffer))
    {
        // Not enough bytes.
        // Discard them.
        discard_excess_rx_data(_lora_interface, packet_size);
        return;
    }
    if (packet_size > sizeof(_receive_buffer))
    {
        // TODO: Need to know if this can happen.
        // For now still read one location's worth of
        // data and discard it later.
        DBG_LOG_ERROR("LL: packet too large\n");
    }

    // read packet
    unsigned i = 0;
    while (_lora_interface.available() && i < sizeof(_receive_buffer)) {
        int byte_read = _lora_interface.read();
        if (byte_read < 0 || byte_read > UINT8_MAX)
        {
            // Give up trying to read this and exit.
            DBG_LOG_ERROR("LL: failed to read byte\n");
            return;
        }
        _receive_buffer[i] = (uint8_t)byte_read;
        i++;
    }

    if (i != sizeof(_receive_buffer))
    {
        DBG_LOG_ERROR("LL: read too few bytes\n");
        return;
    }

    if (packet_size > sizeof(_receive_buffer))
    {
        // There was excess data
        // Discard it.
        discard_excess_rx_data(_lora_interface, packet_size - sizeof(_receive_buffer));
    }

    if (!_location.un_pack(_receive_buffer, sizeof(_receive_buffer)))
    {
        // TODO: Log this
        DBG_LOG_ERROR("LL: recvd corrupt location\n");
        return;
    }

    if (_location.get_station_id() != _station_id)
    {
        // Don't store our own location as a peer location.
        DBG_LOG_INFO("LL: storing loc (%d) %u:\n", _lora_interface.packetRssi(), _location.get_station_id());
#if DBG_LOG_LEVEL >= DBG_LOG_LEVEL_INFO
        Serial.print(_location.get_latitude(), 5);
        Serial.print(",");
        Serial.println(_location.get_longitude(), 5);
#endif // DBG_LOG_LEVEL >= DBG_LOG_LEVEL_INFO
        _peer_locations.store_location(_lora_interface.packetRssi(), _lora_interface.packetSnr(), _location);
    }
    else
    {
        DBG_LOG_INFO("LL: skipping own loc\n");
    }
}

void LocationListener::do_work()
{
    /*
     * TODO: Learn about errors and what to do with these functions.
     * Also need to configure HW to do as much error checking and correction as it can.
     * packetRssi()
     * packetSnr()
     * packetFrequencyError()
     */

    unsigned packet_size = _lora_interface.parsePacket();
    if (packet_size > 0) {
        on_receive(packet_size);
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
