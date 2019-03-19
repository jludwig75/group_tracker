#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LoRa.h>

#include "group_locator.h"
#include "lora_interface.h"
#include "hw_lora_interface.h"
#include "debug_log.h"

#define LORA_SS_PIN         8
#define LORA_RESET_PIN      4
#define LORA_DIO0_PIN       7
#define GPS_RX_PIN          10
#define GPS_TX_PIN          11
#define PPS_INTERRUPT_PIN   2

#define STATIONS_PER_GROUP          30
#define TX_TIME_SEC                 1
#define PEER_LOCATIONS_TO_STORE     4
#define MAX_PEER_LOCATIONS_TO_SEND  4

const uint32_t station_id = 4;

#ifndef digitalPinToInterrupt
#define digitalPinToInterrupt(x)    x
#endif  // digitalPinToInterrupt

HwLoRaInterface lora(LoRa);

SoftwareSerial gps_serial_port(GPS_RX_PIN, GPS_TX_PIN);

GroupLocator locator(station_id,
                     STATIONS_PER_GROUP,
                     &gps_serial_port,
                     lora,
                     TX_TIME_SEC,
                     PEER_LOCATIONS_TO_STORE,
                     MAX_PEER_LOCATIONS_TO_SEND);

DebugLog debug_log(Serial);

void pps_interrupt()
{
    locator.on_pps_interrupt();
}

void setup()
{
    Serial.begin(115200);
    
    DBG_LOG_INFO("Group Locator starting...\n");

    // Initialize LORA
    LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);
    if (!LoRa.begin(433E6)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    
    // Start the locator
    locator.begin();

    // Setup the PPS interrupt
    pinMode(PPS_INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PPS_INTERRUPT_PIN), pps_interrupt, RISING);

    DBG_LOG_INFO("Group Locator started\n");
}

void loop()
{
    locator.on_loop();
}
