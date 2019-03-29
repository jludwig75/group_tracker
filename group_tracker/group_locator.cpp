#include "group_locator.h"

#define DBG_LOG_LEVEL   DBG_LOG_LEVEL_INFO
#include "debug_log.h"

#include "lora_interface.h"

#define GPS_IDX         0
#define LISTENER_IDX    1
#define SENDER_IDX      2


GroupLocator::GroupLocator(uint32_t station_id,
                           unsigned stations_per_group,
                           SoftwareSerial *gps_serial_interface,
                           LoRaInterface &lora_interface,
                           unsigned transmission_time_sec,
                           bool recevie_on_interrupt) :
    _station_id(station_id),
    _stations_per_group(stations_per_group),
    _active_worker(LISTENER_IDX),
    _transmission_time_sec(transmission_time_sec),
    _enable_communication(false),
    _gps_clock(),
    _location_tracker(station_id, gps_serial_interface, _gps_clock),
    _location_listener(station_id, lora_interface),
    _location_sender(lora_interface, _location_tracker, _location_listener),
    _lora_interface(lora_interface),
    _recevie_on_interrupt(recevie_on_interrupt),
    _workers()
{
    _workers[GPS_IDX] = &_location_tracker;
    _workers[LISTENER_IDX] = _recevie_on_interrupt ? NULL : &_location_listener;
    _workers[SENDER_IDX] = &_location_sender;
}

void GroupLocator::begin()
{
    DBG_LOG_INFO("GL: starting\n");
    _location_tracker.begin();
}

uint32_t GroupLocator::get_active_station_number(uint32_t second) const
{
    return (second / _transmission_time_sec) % _stations_per_group;
}

void GroupLocator::on_pps_interrupt()
{
    DBG_LOG_DEBUG("GL: PPS\n");
    // We know we have a fix if we got a PPS interrupt
    _gps_clock.on_pps_pulse();
    // Don't do anything if the time has not yet been set.
    if (_gps_clock.is_time_set())
    {
        uint32_t second = _gps_clock.get_millis() / 1000;

        DBG_LOG_DEBUG("active station = %lu, station ID = %lu\n", get_active_station_number(second), _station_id);

        if (_station_id == get_active_station_number(second))
        {
            // We're up. Start sending our packets
            DBG_LOG_DEBUG("GL: sending\n");
            digitalWrite(LED_BUILTIN, HIGH);
            _active_worker = SENDER_IDX;
            if (_recevie_on_interrupt)
            {
                _lora_interface.idle();
            }
            _location_sender.start_sending_locations(second);
        }
        else
        {
            // Not our turn. Enable listener
            DBG_LOG_DEBUG("GL: listening\n");
            digitalWrite(LED_BUILTIN, LOW);
            _active_worker = LISTENER_IDX;
            if (_recevie_on_interrupt)
            {
                _lora_interface.receive();
            }
        }
        
        _enable_communication = true;
    }
    else
    {
        DBG_LOG_DEBUG("GL: PPS time not set\n");
    }
    
}

void GroupLocator::on_lora_receive(int packet_size)
{
    _location_listener.on_receive(packet_size);
}


void GroupLocator::on_loop()
{
    _workers[GPS_IDX]->do_work();
    if (_enable_communication)
    {
        Worker *worker = _workers[_active_worker];
        if (worker)
        {
            _workers[_active_worker]->do_work();
        }
    }
}

void GroupLocator::set_dummy_location(float latitude, float longitude)
{
    _location_tracker.set_dummy_location(latitude, longitude);
}

void GroupLocator::set_dummy_time(time_t time)
{
    _gps_clock.set_gps_time(time);
}
