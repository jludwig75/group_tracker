#include "location.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock-global/gmock-global.h>


#include "debug_log.h"
#include <stdarg.h>

class Print {
    int dummy;
} dummy;

DebugLog debug_log(dummy);

DebugLog::DebugLog(Print & printer) :
    _printer(printer)
{
}

void DebugLog::info(const char *format, ...)
{
    va_list al;

    va_start(al, format);

    vprintf(format, al);

    va_end(al);
}

void DebugLog::debug(const char *format, ...)
{
    va_list al;
    
    va_start(al, format);

    vprintf(format, al);

    va_end(al);
}

void DebugLog::error(const char *format, ...)
{
    va_list al;
    
    va_start(al, format);

    vprintf(format, al);

    va_end(al);
}

TEST(Location_Test, validates_valid_location)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 41.67;
    Location location(station_id, timestamp, accuracy, longitude, latitude);
    ASSERT_TRUE(location.validate());
}

TEST(Location_Test, validation_catches_invalid_latitude)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 90.67;
    Location location(station_id, timestamp, accuracy, longitude, latitude);
    ASSERT_FALSE(location.validate());
}

TEST(Location_Test, validation_catches_invalid_longitude)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -180.01;
    float latitude = 40.67;
    Location location(station_id, timestamp, accuracy, longitude, latitude);
    ASSERT_FALSE(location.validate());
}

TEST(Location_Test, validation_catches_excessive_hop_count)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 41.67;
    Location location(station_id, timestamp, accuracy, longitude, latitude);
    for(int i = 0; i < MAX_HOP_COUNT; i++)
    {
        location.increment_hop_count();
    }
    ASSERT_TRUE(location.validate());
    location.increment_hop_count();
    ASSERT_FALSE(location.validate());
}

TEST(Location_Test, pack_fails_on_null_buffer_or_too_small)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 41.67;
    uint8_t pack_buffer[LOCATION_BLOB_BYES];
    Location location(station_id, timestamp, accuracy, longitude, latitude);

    ASSERT_FALSE(location.pack(NULL, sizeof(pack_buffer)));
    ASSERT_FALSE(location.pack(pack_buffer, sizeof(pack_buffer) - 1));
}

TEST(Location_Test, unpack_fails_on_null_buffer_or_too_small)
{
    uint8_t pack_buffer[LOCATION_BLOB_BYES];
    Location location;

    ASSERT_FALSE(location.un_pack(NULL, sizeof(pack_buffer)));
    ASSERT_FALSE(location.un_pack(pack_buffer, sizeof(pack_buffer) - 1));
}

TEST(Location_Test, unpack_fails_on_invalid_location)
{
    uint8_t pack_buffer[LOCATION_BLOB_BYES];
    Location location;

    memset(pack_buffer, 0xFF, sizeof(pack_buffer));

    ASSERT_FALSE(location.un_pack(pack_buffer, sizeof(pack_buffer)));
}

TEST(Location_Test, pack_fails_with_invalid_location)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 91.67;
    uint8_t pack_buffer[LOCATION_BLOB_BYES];
    Location location(station_id, timestamp, accuracy, longitude, latitude);

    ASSERT_FALSE(location.pack(pack_buffer, sizeof(pack_buffer)));
}

TEST(Location_Test, Location_pack_and_unpack)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 41.67;
    Location location(station_id, timestamp, accuracy, longitude, latitude);

    ASSERT_TRUE(location.validate());

    ASSERT_EQ(station_id, location.get_station_id());
    ASSERT_EQ(timestamp, location.get_timestamp());
    ASSERT_EQ(accuracy, location.get_accuracy());

    uint8_t pack_buffer[LOCATION_BLOB_BYES];

    ASSERT_TRUE(location.pack(pack_buffer, sizeof(pack_buffer)));

    Location location2;

    ASSERT_TRUE(location2.un_pack(pack_buffer, sizeof(pack_buffer)));

    ASSERT_TRUE(location2.validate());

    ASSERT_EQ(station_id, location2.get_station_id());
    ASSERT_EQ(2 * (timestamp / 2), location2.get_timestamp());
    ASSERT_EQ(accuracy, location2.get_accuracy());

    ASSERT_TRUE(fabs(longitude - location2.get_longitude()) < 0.0005);
    ASSERT_TRUE(fabs(latitude - location2.get_latitude()) < 0.0005);
}

TEST(Location_Test, Can_pack_and_un_pack_location_at_coord_extreems)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    uint8_t pack_buffer[LOCATION_BLOB_BYES];
    float longitude;
    float latitude;
    Location location;
    Location location2;

    longitude = -179.9999;
    latitude = -90;
    location = Location(station_id, timestamp, accuracy, longitude, latitude);
    ASSERT_TRUE(location.pack(pack_buffer, sizeof(pack_buffer)));
    //ASSERT_TRUE(location2.un_pack(pack_buffer, sizeof(pack_buffer)));
    //ASSERT_TRUE(fabs(longitude - location2.get_longitude()) < 0.0005);
    //ASSERT_TRUE(fabs(latitude - location2.get_latitude()) < 0.0005);

}