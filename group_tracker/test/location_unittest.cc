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



class Location_Test : public GlobalMockTest
{
    virtual void SetUp()
    {
    }
    virtual void testSetUp()
    {
    }
    virtual void TearDown()
    {
    }

};

TEST_F(Location_Test, Location_pack_and_unpack)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -179.9999;
    float latitude = 41.67;
    Location location(station_id, timestamp, accuracy, longitude, latitude);

    ASSERT_EQ(station_id, location.get_station_id());
    ASSERT_EQ(timestamp, location.get_timestamp());
    ASSERT_EQ(accuracy, location.get_accuracy());

    uint8_t pack_buffer[LOCATION_BLOB_BYES];

    ASSERT_TRUE(location.pack(pack_buffer, sizeof(pack_buffer)));

    Location location2;

    ASSERT_TRUE(location2.un_pack(pack_buffer, sizeof(pack_buffer)));

    ASSERT_EQ(station_id, location2.get_station_id());
    ASSERT_EQ(2 * (timestamp / 2), location2.get_timestamp());
    ASSERT_EQ(accuracy, location2.get_accuracy());

    ASSERT_TRUE(fabs(longitude - location2.get_longitude()) < 0.0005);
    ASSERT_TRUE(fabs(latitude - location2.get_latitude()) < 0.0005);
}