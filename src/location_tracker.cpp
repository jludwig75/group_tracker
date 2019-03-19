#include "location_tracker.h"

#include "gps_clock.h"
#include "debug_log.h"

#include <stdlib.h>
#include <SoftwareSerial.h>


LocationTracker::LocationTracker(uint32_t station_id, SoftwareSerial *gps_serial_interface, GpsClock &gps_clock) :
    Worker(),
    _station_id(station_id),
    _gps_serial_interface(gps_serial_interface),
    _gps(gps_serial_interface),
    _gps_clock(gps_clock),
    _current_location()
{
}

void LocationTracker::begin()
{
    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    _gps.begin(9600);
    
    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    _gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    // uncomment this line to turn on only the "minimum recommended" data
    //_gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
    // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
    // the parser doesn't care about other sentences at this time
    
    // Set the update rate
    _gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
    // For the parsing code to work nicely and have time to sort thru the data, and
    // print it out we don't suggest using anything higher than 1 Hz

    // Request updates on antenna status, comment out to keep quiet
    //_gps.sendCommand(PGCMD_ANTENNA);

    delay(1000);
    // Ask for firmware version
    _gps_serial_interface->println(PMTK_Q_RELEASE);
}

void LocationTracker::get_current_location(Location &location) const
{
    // Synchronize with timer
    noInterrupts();
    location = _current_location;
    interrupts();
}

void LocationTracker::work_func()
{
    while (_gps.read() != 0)
    {
        if (_gps.newNMEAreceived())
        {
            boolean date_and_time_updated = false;
            if (!_gps.parse(_gps.lastNMEA(), &date_and_time_updated))   // this also sets the newNMEAreceived() flag to false
            {
                DBG_LOG_ERROR("LT: error parsing NMEA: %s\n", _gps.lastNMEA());
                return;  // we can fail to parse a sentence in which case we should just wait for the next sentence.
            }
            if (_gps.fix > 0 && date_and_time_updated)
            {
                _gps_clock.set_gps_time(2000 + _gps.year, _gps.month, _gps.day, _gps.hour, _gps.minute, _gps.seconds);
                
                // Get the timestamp from the GPS data.
                tmElements_t tm;
                tm.Year = CalendarYrToTm(_gps.year + 2000);
                tm.Month = _gps.month;
                tm.Day = _gps.day;
                tm.Hour = _gps.hour;
                tm.Minute = _gps.minute;
                tm.Second = _gps.seconds;
                time_t reading_time = makeTime(tm); // TODO: What epoch? Who hanels beginnig of yeat epoch? Probably this code.
                DBG_LOG_INFO("LT: updated location\n");
                _current_location = Location(_station_id, reading_time, (uint8_t)_gps.fix * 16 + _gps.fixquality, _gps.longitude, _gps.latitude);
            }
        }
    }
}
