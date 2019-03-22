#include "location.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock-global/gmock-global.h>


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

TEST_F(Location_Test, Location_store_and_retrieve_timestamp)
{
    uint32_t station_id = 17;
    uint32_t timestamp = 5435437;
    uint8_t accuracy = 47;
    float longitude = -110.08;
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
    printf("%f, %f\n", location2.get_longitude(), location2.get_latitude());
}