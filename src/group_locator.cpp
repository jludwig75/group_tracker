#include "group_locator.h"


GroupLocator::GroupLocator(uint32_t station_id, SoftwareSerial *gps_serial_interface, LoRaInterface &lora_interface) :
    _gps_clock(),
    _location_tracker(station_id, gps_serial_interface, _gps_clock),
    _location_listener(station_id, lora_interface),
    _location_sender(station_id, lora_interface, _location_tracker, _location_listener, 8, 50000),
    _workers()
{
    _workers[0] = &_location_sender;
    _workers[1] = &_location_tracker;
    _workers[2] = &_location_listener;
}

void GroupLocator::begin()
{
    _location_tracker.begin();
}

void GroupLocator::on_pps_interrupt()
{
    // We know we have a fix if we got a PPS interrupt
    _gps_clock.on_pps_pulse();
    // Don't do anything if the time has not yet been set.
    if (_gps_clock.is_time_set())
    {
        uint32_t second = _gps_clock.get_millis() / 1000;
        _location_sender.send_locations(second);
    }
}

void GroupLocator::on_loop()
{
    for(unsigned i = 0; i < NUM_WORKERS; i++)
    {
        _workers[i]->do_work();
    }
}
