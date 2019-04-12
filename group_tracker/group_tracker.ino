#include <Arduino.h>
#ifndef ESP32
#include <SoftwareSerial.h>
#else   // ESP32
#include <HardwareSerial.h>
#endif  // ESP32
#include <LoRa.h>
#ifdef  NO_GPS
#include <Timer.h>
#endif  // NO_GPS

#include "group_locator.h"
#include "lora_interface.h"
#include "hw_lora_interface.h"

#define DBG_LOG_LEVEL   DBG_LOG_LEVEL_INFO
#include "debug_log.h"

#ifdef ESP32

#define LORA_SS_PIN         18
#define LORA_RESET_PIN      16
#define LORA_DIO0_PIN       26

#define GPS_RX_PIN          16
#define GPS_TX_PIN          17
#define PPS_INTERRUPT_PIN   36

#else   // ESP32

#define LORA_SS_PIN         8
#define LORA_RESET_PIN      4
#define LORA_DIO0_PIN       7

#define GPS_RX_PIN          10
#define GPS_TX_PIN          11
#define PPS_INTERRUPT_PIN   2

#endif  // ESP32

#define STATIONS_PER_GROUP          4
#define TX_TIME_SEC                 2

#define STATION_ID  0


HwLoRaInterface lora(LoRa);

#ifndef ESP32
SoftwareSerial gps_serial_port(GPS_RX_PIN, GPS_TX_PIN);
#else   // ESP32
HardwareSerial gps_serial_port(2);
#endif  // ESP32

GroupLocator locator(STATION_ID,
                     STATIONS_PER_GROUP,
                     &gps_serial_port,
                     lora,
                     TX_TIME_SEC);

DebugLog debug_log(Serial);

void pps_interrupt()
{
    locator.on_pps_interrupt();
}

void on_lora_receive(int pack_size)
{
    locator.on_lora_receive(pack_size);
}

//#define NO_GPS

#ifdef  NO_GPS
Timer t;

void fake_pps()
{
    locator.set_dummy_time(millis() / 1000);
    pps_interrupt();
}

void setup_no_gps()
{
    locator.set_dummy_time(millis() / 1000); // Anything greater than 0
    locator.set_dummy_location(40.4144428, -111.8307572);
    t.every(1000, fake_pps);
}
#else   // NO_GPS
void setup_gps()
{
    // Setup the PPS interrupt
    pinMode(PPS_INTERRUPT_PIN, INPUT);
    attachInterrupt(digitalPinToInterrupt(PPS_INTERRUPT_PIN), pps_interrupt, RISING);
}
#endif  // NO_GPS


void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.begin(115200);
    
    DBG_LOG_INFO("Group Locator starting...\n");

    // Initialize LORA
    LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);
    if (!LoRa.begin(433E6)) {
        DBG_LOG_ERROR("Starting LoRa failed!");
        while (1);
    }

    LoRa.setTxPower(20);
    LoRa.setSpreadingFactor(8);
    LoRa.setSignalBandwidth(62.5E3);
    LoRa.onReceive(on_lora_receive);
    LoRa.setCodingRate4(8);

    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
#ifdef  ESP32
    gps_serial_port.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
#else   // ESP32
    gps_serial_port.begin(9600);
#endif  // ESP32
    
    // Start the locator
    locator.begin();

#ifdef  NO_GPS
    setup_no_gps();
#else   // NO_GPS
    setup_gps();
#endif  // NO_GPS

    DBG_LOG_INFO("Group Locator started\n");
    digitalWrite(LED_BUILTIN, LOW);

    delay(750);

    for (uint8_t i = 0; i < 3; i++)
    {
        delay(250);

        digitalWrite(LED_BUILTIN, HIGH);
        delay(250);
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void loop()
{
#ifdef  NO_GPS
    t.update();
#endif  // NO_GPS
    locator.on_loop();
}
