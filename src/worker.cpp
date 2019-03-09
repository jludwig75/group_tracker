#include "worker.h"

#include <Arduino.h>

Worker::Worker(unsigned long max_time_slice_us) :
    _max_time_slice_us(max_time_slice_us)
{

}

Worker::~Worker()
{
}

bool Worker::should_yield() const
{
    return (micros() - _work_start_time) > _max_time_slice_us;
}

void Worker::do_work()
{
    _work_start_time = micros();

    work_func();
}
