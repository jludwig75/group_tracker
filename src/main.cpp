#include <Arduino.h>
#include <SoftwareSerial.h>
#include <LoRa.h>

#include "group_locator.h"
#include "lora_interface.h"
#include "hw_lora_interface.h"

#define GPS_RX_PIN  8
#define GPS_TX_PIN  9
#define PPS_INTERRUPT_PIN 2

const uint32_t station_id = 0;


HwLoRaInterface lora(LoRa);

SoftwareSerial gps_serial_port(GPS_RX_PIN, GPS_TX_PIN);

GroupLocator locator(station_id, &gps_serial_port, lora);

void pps_interrupt()
{
    locator.on_pps_interrupt();
}

void setup()
{
    Serial.begin(9600);
    
    // Start the locator
    locator.begin();

    // Setup the PPS interrupt
    pinMode(PPS_INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PPS_INTERRUPT_PIN), pps_interrupt, RISING);
}

void loop()
{
    locator.on_loop();
}