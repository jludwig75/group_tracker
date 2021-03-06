#include "location_tracker.h"

#include "gps_clock.h"

#define DBG_LOG_LEVEL   DBG_LOG_LEVEL_INFO
#include "debug_log.h"

#include <stdlib.h>


LocationTracker::LocationTracker(uint32_t station_id, Stream *gps_serial_interface, GpsClock &gps_clock) :
    _station_id(station_id),
    _gps_serial_interface(gps_serial_interface),
    _gps(gps_serial_interface),
    _gps_clock(gps_clock),
    _current_location()
{
}

void LocationTracker::begin()
{
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

void LocationTracker::do_work()
{
    _gps.read();

    if (_gps.newNMEAreceived())
    {
        boolean date_and_time_updated = false;
        if (!_gps.parse(_gps.lastNMEA(), &date_and_time_updated))   // this also sets the newNMEAreceived() flag to false
        {
            if (strlen(_gps.lastNMEA()) >= 6 && (strncmp(_gps.lastNMEA(), "$GPGGA", 6) == 0 || strncmp(_gps.lastNMEA(), "$GPRMC", 6) == 0))
            {
                DBG_LOG_ERROR("LT: parse err: %s\n", _gps.lastNMEA());
            }
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
            time_t reading_time = makeTime(tm);
            tm.Month = 1;
            tm.Day = 1;
            tm.Hour = 0;
            tm.Minute = 0;
            tm.Second = 0;
            time_t epoch_start = makeTime(tm);
            Location::set_epoch(epoch_start);
            DBG_LOG_INFO("LT: updated loc\n");
#if DBG_LOG_LEVEL >= DBG_LOG_LEVEL_DEBUG
            Serial.print(_gps.latitudeDegrees, 5);
            Serial.print(",");
            Serial.println(_gps.longitudeDegrees, 5);
#endif // DBG_LOG_LEVEL >= DBG_LOG_LEVEL_DEBUG
            _gps_location = Location(_station_id, reading_time, (uint8_t)_gps.fix * 16 + _gps.fixquality, _gps.longitudeDegrees, _gps.latitudeDegrees);
            if (_gps_location.validate())
            {
                _current_location = _gps_location;
            }
            else
            {
                DBG_LOG_ERROR("LT: Invalid GPS loc\n");
            }
        }
    }
}

void LocationTracker::set_dummy_location(float latitude, float longitude)
{
    _current_location = Location(_station_id, _gps_clock.get_time(), 1 * 16 + 2, _gps.longitude, _gps.latitude);
}
