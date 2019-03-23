#include "lora_location_sender.h"

#include "location.h"
#include "mocks/lora_interface_mock.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <gmock-global/gmock-global.h>


TEST(LoRaLocationSender_Test, returns_retry_if_packet_begin_returns_0)
{
    LoRaInterfaceMock lora(1);
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -100.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Retry ,sender.send_location(&location));
}

TEST(LoRaLocationSender_Test, returns_success_if_no_retries_and_no_errors)
{
    LoRaInterfaceMock lora(0);
    LoRaLocationSender sender(lora);

    Location location(0, 0, 0, -100.576, 43.232);
    ASSERT_EQ(LoRaLocationSender::Success ,sender.send_location(&location));
}