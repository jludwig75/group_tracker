#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

typedef bool boolean;

class HardwareSerial;

#define LED_BUILTIN    3
#define HIGH    1
#define LOW     0

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void digitalWrite(uint8_t pin, uint8_t value);