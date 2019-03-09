#include <Arduino.h>
#include <SoftwareSerial.h>

#include "group_locator.h"
#include "lora_interface.h"

#define GPS_TX_PIN  1
#define GPS_RX_PIN  2


class DummyLoRaInterface : public LoRaInterface
{
public:
    virtual int beginPacket(int implicitHeader = false)
    {
      return -1;
    }
    virtual int endPacket()
    {
      return -1;
    }

    virtual int parsePacket(int size = 0)
    {
      return -1;
    }
    virtual int packetRssi()
    {
      return -1;
    }
    virtual float packetSnr()
    {
      return -1;
    }
    virtual long packetFrequencyError()
    {
      return -1;
    }

    // from Print
    virtual unsigned write(uint8_t byte)
    {
      return 0;
    }
    virtual unsigned write(const uint8_t *buffer, unsigned size)
    {
      return 0;
    }

    // from Stream
    virtual int available()
    {
      return -1;
    }
    virtual int read()
    {
      return -1;
    }
    virtual int peek()
    {
      return -1;
    }
    virtual void flush()
    {
    }
} LoRa2;

SoftwareSerial gps_serial_port(GPS_RX_PIN, GPS_TX_PIN);

GroupLocator locator(gps_serial_port, LoRa2);

bool locator_started = false;

void setup() {
  Serial.begin(9600);
  locator_started = locator.begin();
  if (!locator_started)
  {
    Serial.println("Failed to start locator");
    // TODO: Probably add retries
  }

  // TODO: Setup PPS interrupt
}

void loop() {
  locator.on_loop();
}