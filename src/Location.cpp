#include "location.h"

#include <Arduino.h>


#define SECONDS_PER_TS_UINT 2

struct LocationBlob
{
    uint32_t station_id : 24;
    uint32_t hop_count : 8;

    uint32_t timestamp : 24;
    uint32_t accuracy : 8;

    uint32_t longitude : 24;
    uint32_t latitude_hi : 8;

    uint32_t latitude_lo : 16;
    uint32_t crc : 16;
};

#define COORD_MIN   -180UL
#define COORD_MAX    180UL
#define ACCURACY_MULTIPLIER 100000UL

static uint32_t map_coord_to_blob_space(float coord)
{
    uint32_t _coord = (coord * ACCURACY_MULTIPLIER);

    return map(_coord,
                COORD_MIN * ACCURACY_MULTIPLIER,
                COORD_MAX * ACCURACY_MULTIPLIER,
                0,
                (1UL << 24) - 1);
}

static float map_blob_value_to_coord_space(uint32_t blob_value)
{
    uint32_t _coord = map(blob_value,
                            0,
                            (1UL >> 24) - 1,
                            COORD_MIN * ACCURACY_MULTIPLIER,
                            COORD_MAX * ACCURACY_MULTIPLIER);

    return ((float)_coord) / ACCURACY_MULTIPLIER;
}


Location::Location(uint32_t station_id, uint32_t timestamp, uint8_t accuracy, float longitude, float latitude) :
    _station_id(station_id),
    _timestamp(timestamp),
    _hop_count(0),
    _accuracy(accuracy),
    _longitude(longitude),
    _latitude(latitude)
{
}

Location::Location() :
    _station_id(UINT32_MAX),
    _timestamp(UINT32_MAX),
    _hop_count(UINT8_MAX),
    _accuracy(UINT8_MAX),
    _longitude(1000),
    _latitude(1000)
{
}

bool Location::un_pack(const uint8_t *blob_buffer, unsigned buffer_bytes)
{
    if (blob_buffer == NULL || buffer_bytes < get_blob_bytes())
    {
        return false;
    }

    const LocationBlob *blob = (const LocationBlob *)blob_buffer;

    // TODO: Sanitize parameters (timestamp, longitude, latitude, accuracy) and check CRC.

    _station_id = blob->station_id;
    _timestamp = blob->timestamp * SECONDS_PER_TS_UINT; // TODO: Add year start
    _accuracy = blob->accuracy;
    _hop_count = blob->hop_count;
    _latitude = map_blob_value_to_coord_space(((uint32_t)blob->latitude_hi) << 16 | (uint32_t)blob->latitude_lo);
    _longitude = map_blob_value_to_coord_space(blob->longitude);

    return true;
}

uint32_t Location::get_station_id() const
{
    return _station_id;
}

uint32_t Location::get_timestamp() const
{
    return _timestamp;
}

uint8_t Location::get_hop_count() const
{
    return _hop_count;
}

uint8_t Location::get_accuracy() const
{
    return _accuracy;
}

float Location::get_logitude() const
{
    return _longitude;
}


float Location::get_latitude() const
{
    return _latitude;
}

void Location::increment_hop_count()
{
    _hop_count += 1;
}

unsigned Location::get_blob_bytes() const
{
    return sizeof(LocationBlob);
}

bool Location::pack(uint8_t *blob_buffer, unsigned buffer_bytes) const
{
    LocationBlob *blob;

    if (buffer_bytes < sizeof(*blob))
    {
        return false;
    }

    blob = (LocationBlob *)blob_buffer;

    blob->station_id = _station_id;
    blob->accuracy = _accuracy;
    blob->hop_count = _hop_count;
    blob->timestamp = _timestamp / SECONDS_PER_TS_UINT; // TODO: Subtract year start
    blob->longitude = map_coord_to_blob_space(_longitude);
    uint32_t latitude = map_coord_to_blob_space(_latitude);
    blob->latitude_lo = latitude & UINT16_MAX;
    blob->latitude_hi = (latitude >> 16) & UINT8_MAX;

    return true;
}
