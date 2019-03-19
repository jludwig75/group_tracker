#include "gps_clock.h"

#include "debug_log.h"


GpsClock::GpsClock() :
  _epoch(0),
  _last_time_set(0),
  _last_micros(0),
  _last_gps_time_sec(0)
{
}

boolean GpsClock::is_time_set() const
{
  return _last_time_set != 0;
}

void GpsClock::set_epoch(uint16_t year)
{
  tmElements_t tm;
  tm.Second = 0;
  tm.Minute = 0;
  tm.Hour = 0;
  tm.Day = 1;
  tm.Month = 1;
  tm.Year = CalendarYrToTm(year);
  _epoch = (uint64_t)makeTime(tm) * 1000000ULL;
}

uint64_t GpsClock::get_micros() const
{
  
  noInterrupts();
  uint64_t m = (uint64_t)_last_time_set * 1000000ULL + (micros() - _last_micros);
  interrupts();
  return m;
}

uint32_t GpsClock::get_millis() const
{
  return (uint32_t)(get_micros() / 1000UL);
}

// TODO: We could also cache the date and allow updates with just the time. Internal to the time-only update
// function we could use the cache date, if it has already been set. We would update more often, because $GPGGA
// has just the time and the date. However, we would have to test it relative to the PPS pulse and make sure our
// rounding tweak in the GPS parser doesn't come into play. It may not be desriable though to use the time-only sentece.
// Of course, it may be more desireable (i.e may not have the rounding issue or coincide better with the PPS pulse).
void GpsClock::set_gps_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
  tmElements_t tm;
  tm.Second = second;
  tm.Minute = minute;
  tm.Hour = hour;
  tm.Day = day;
  tm.Month = month;
  tm.Year = CalendarYrToTm(year);
  time_t t = makeTime(tm);
  set_gps_time(t);
  set_epoch(year);
}

void GpsClock::set_gps_time(time_t t)
{
//    Serial.print("Set GPS time "); Serial.println((unsigned long)t);
  noInterrupts();
  _last_gps_time_sec = t;
  interrupts();
}

void GpsClock::on_pps_pulse()
{
  _last_micros = micros();
  _last_time_set = _last_gps_time_sec + 1;
//    Serial.print("Set clock time to "); Serial.println((unsigned long)_last_time_set);
}

time_t GpsClock::get_time() const
{
  // NOTE: This breaks if the time hasn't been set in a little over an hour,
  // because the microseconds wrap. May be an issue if the wrap happens at
  // any time. I think it needs to be synchronized to ensure this. Well
  // the wrap will result in a larger value for the previous time and the
  // subtraction will wrap too. This should stil work, but synchronization would be
  // good to be sure.
  noInterrupts();
  time_t t = _last_time_set + (micros() - _last_micros) / 1000000UL;
  interrupts();
  
  return t;
}
