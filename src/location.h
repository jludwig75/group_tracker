#pragma once

#include <stdint.h>


class Location
{
public:
    Location(uint32_t station_id, uint32_t timestamp, uint8_t accuracy, float longitude, float latitude);
    Location();

    uint32_t get_station_id() const;
    uint32_t get_timestamp() const;
    uint8_t get_hop_count() const;
    uint8_t get_accuracy() const;
    float get_logitude() const;
    float get_latitude() const;

    bool un_pack(const uint8_t *blob_buffer, unsigned buffer_bytes);

    unsigned get_blob_bytes() const;
    bool pack(uint8_t *blob_buffer, unsigned buffer_bytes) const;

    void increment_hop_count();
    
private:
    uint32_t _station_id;
    uint32_t _timestamp;
    uint8_t _hop_count;
    uint8_t _accuracy;
    float _longitude;
    float _latitude;
};