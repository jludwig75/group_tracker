#pragma once

#include <stdint.h>

class Worker
{
public:
    Worker(uint32_t max_time_slice_us = 50000);
    virtual ~Worker();

    void do_work();

protected:
    bool should_yield() const;
    virtual void work_func() = 0;

    uint32_t _max_time_slice_us;
    uint32_t _work_start_time;
};