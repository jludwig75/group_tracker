#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LoRa.h>

#include "group_locator.h"
#include "lora_interface.h"
#include "hw_lora_interface.h"
#include "debug_log.h"

#define GPS_RX_PIN          8
#define GPS_TX_PIN          9
#define PPS_INTERRUPT_PIN   2

#define STATIONS_PER_GROUP          30
#define TX_TIME_SEC                 1
#define PEER_LOCATIONS_TO_STORE     4
#define MAX_PEER_LOCATIONS_TO_SEND  4

const uint32_t station_id = 0;

#ifndef digitalPinToInterrupt
#define digitalPinToInterrupt(x)    x
#endif  // digitalPinToInterrupt

HwLoRaInterface lora(LoRa);

SoftwareSerial gps_serial_port(GPS_RX_PIN, GPS_TX_PIN);

GroupLocator locator(station_id,
                     STATIONS_PER_GROUP,
                     &gps_serial_port,
                     lora,
                     true,
                     TX_TIME_SEC,
                     PEER_LOCATIONS_TO_STORE,
                     MAX_PEER_LOCATIONS_TO_SEND);

DebugLog debug_log(Serial);

void pps_interrupt()
{
    locator.on_pps_interrupt();
}

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect)
{
    locator.on_gps_timer();
}


void setup()
{
    Serial.begin(9600);
    
    DBG_LOG_INFO("Group Locator starting...\n");
    
    // Start the locator
    locator.begin();

    // Start the GPS timer
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);

    // Setup the PPS interrupt
    pinMode(PPS_INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PPS_INTERRUPT_PIN), pps_interrupt, RISING);

    DBG_LOG_INFO("Group Locator started\n");
}

void loop()
{
    locator.on_loop();
}