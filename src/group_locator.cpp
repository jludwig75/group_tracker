#include "group_locator.h"


GroupLocator::GroupLocator(Stream & gps_serial_interface, LoRaInterface &lora_interface) :
    _location_tracker(gps_serial_interface),
    _location_listener(lora_interface),
    _location_sender(lora_interface, _location_tracker, _location_listener, 8, 50000),
    _workers()
{

}

bool GroupLocator::begin()
{
    return false;
}

void GroupLocator::on_pps_interrupt()
{

}

void GroupLocator::on_loop()
{

}
