#include "group_locator.h"

#define LISTENER_IDX    0
#define SENDER_IDX      1
#define GPS_IDX         2


GroupLocator::GroupLocator(uint32_t station_id,
                           unsigned stations_per_group,
                           SoftwareSerial *gps_serial_interface,
                           LoRaInterface &lora_interface,
                           bool use_timer_for_gps,
                           unsigned transmission_time_sec) :
    _station_id(station_id),
    _stations_per_group(stations_per_group),
    _active_worker(LISTENER_IDX),
    _use_timer_for_gps(use_timer_for_gps),
    _transmission_time_sec(transmission_time_sec),
    _gps_clock(),
    _location_tracker(station_id, gps_serial_interface, _gps_clock),
    _location_listener(station_id, lora_interface),
    _location_sender(station_id, lora_interface, _location_tracker, _location_listener, 8, 50000),
    _workers()
{
    _workers[LISTENER_IDX] = &_location_listener;
    _workers[SENDER_IDX] = &_location_sender;
    _workers[GPS_IDX] = &_location_tracker;
}

void GroupLocator::begin()
{
    _location_tracker.begin();
}

uint32_t GroupLocator::get_active_station_number(uint32_t second) const
{
    return (second / _transmission_time_sec) % _stations_per_group;
}

void GroupLocator::on_pps_interrupt()
{
    // We know we have a fix if we got a PPS interrupt
    _gps_clock.on_pps_pulse();
    // Don't do anything if the time has not yet been set.
    if (_gps_clock.is_time_set())
    {
        uint32_t second = _gps_clock.get_millis() / 1000;

        if (_station_id == get_active_station_number(second))
        {
            // We're up. Start sending our packets
            _active_worker = SENDER_IDX;
            _location_sender.start_sending_locations(second);
        }
        else
        {
            // Not our turn. Enable listener
            _active_worker = LISTENER_IDX;
        }
        
    }
}

void GroupLocator::on_loop()
{
    _workers[_active_worker]->do_work();
    if (!_use_timer_for_gps)
    {
        _workers[GPS_IDX]->do_work();
    }
}

void GroupLocator::on_gps_timer()
{
    // assert _use_timer_for_gps == true
    // TODO: This ^^^ is unexpected. Log this

    _workers[GPS_IDX]->do_work();
}
