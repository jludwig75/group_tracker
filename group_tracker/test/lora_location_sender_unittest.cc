#include "lora_location_sender.h"

#include "location.h"
#include "mocks/lora_interface_mock.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock-global/gmock-global.h>


TEST(LoRaLocationSender_Test, returns_success_if_no_retries_and_no_errors)
{
    LoRaInterfaceMock lora;
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -100.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Success ,sender.send_location(&location));

    // Make sure the packet was written
    ASSERT_EQ(LOCATION_BLOB_BYES, lora.get_num_bytes_written());
}

TEST(LoRaLocationSender_Test, returns_retry_if_packet_begin_returns_0)
{
    LoRaInterfaceMock lora(1);
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -100.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Retry ,sender.send_location(&location));

    // Make sure nothing was written
    ASSERT_EQ(0, lora.get_num_bytes_written());
}

TEST(LoRaLocationSender_Test, returns_error_if_location_pack_fails)
{
    LoRaInterfaceMock lora;
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -180.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Error ,sender.send_location(&location));

    // Make sure nothing was written
    ASSERT_EQ(0, lora.get_num_bytes_written());
}

TEST(LoRaLocationSender_Test, returns_error_if_write_fails)
{
    LoRaInterfaceMock lora(0, true);
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -100.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Error ,sender.send_location(&location));

    // Make sure nothing was written
    ASSERT_EQ(0, lora.get_num_bytes_written());
}

TEST(LoRaLocationSender_Test, returns_error_if_end_packet_fails)
{
    LoRaInterfaceMock lora(0, false, true);
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -100.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Error ,sender.send_location(&location));

    // Make sure the packet was written
    ASSERT_EQ(LOCATION_BLOB_BYES, lora.get_num_bytes_written());
}
