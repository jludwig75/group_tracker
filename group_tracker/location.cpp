#include "location.h"

#include <Arduino.h>

#define DBG_LOG_LEVEL   DBG_LOG_LEVEL_OFF
#include "debug_log.h"

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

static_assert(sizeof(LocationBlob) == LOCATION_BLOB_BYES, "\"LocationBlob\" data structure size not as expected");

#define COORD_MIN   -180L
#define COORD_MAX    180L
#define ACCURACY_MULTIPLIER 100000L

static int32_t map32(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max)
{
    int32_t ret = (int32_t)((int64_t)(x - in_min) * (int64_t)(out_max - out_min) / (int64_t)(in_max - in_min) + (int64_t)out_min);
    DBG_LOG_DEBUG("x=%d, in_min=%d, in_max=%d, out_min=%d, out_max=%d, ret=%d\n", x, in_min, in_max, out_min, out_max, ret);
    return ret;
}


static uint32_t map_coord_to_blob_space(float coord)
{
    float coord2 = coord * ACCURACY_MULTIPLIER;
    int32_t _coord = (int32_t)coord2;


    uint32_t blob_value = map32(_coord,
                COORD_MIN * ACCURACY_MULTIPLIER,
                COORD_MAX * ACCURACY_MULTIPLIER,
                0,
                (1UL << 24) - 1);

    DBG_LOG_DEBUG("%s: ACCURACY_MULTIPLIER = %d, coord=%f, coord2=%f, _coord = %d, blob_value = %lu\n", __func__, ACCURACY_MULTIPLIER, coord, coord2, _coord, blob_value);

    return blob_value;
}

static float map_blob_value_to_coord_space(uint32_t blob_value)
{
    int32_t _coord = map32(blob_value,
                            0,
                            (1L << 24) - 1,
                            COORD_MIN * ACCURACY_MULTIPLIER,
                            COORD_MAX * ACCURACY_MULTIPLIER);

    float coord = ((float)_coord) / ACCURACY_MULTIPLIER; 

    DBG_LOG_DEBUG("%s: blob_value = %u, _coord = %d, coord = %f\n", __func__, blob_value, _coord, coord);

    return coord;
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

bool Location::validate() const
{
    if (_hop_count > MAX_HOP_COUNT)
    {
        return false;
    }

    if (_latitude < -90 || _latitude > 90 || _longitude < -180 || _longitude > 180)
    {
        return false;
    }

    return true;
}


bool Location::un_pack(const uint8_t *blob_buffer, unsigned buffer_bytes)
{
    if (blob_buffer == NULL || buffer_bytes < LOCATION_BLOB_BYES)
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

    return validate();
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

float Location::get_longitude() const
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

bool Location::pack(uint8_t *blob_buffer, unsigned buffer_bytes) const
{
    if (!blob_buffer || !validate())
    {
        return false;
    }

    LocationBlob *blob = (LocationBlob *)blob_buffer;

    if (buffer_bytes < sizeof(*blob))
    {
        return false;
    }

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
