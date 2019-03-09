#pragma once


class Worker
{
public:
    Worker(unsigned long max_time_slice_us);
    virtual ~Worker();

    void do_work();

protected:
    bool should_yield() const;
    virtual void work_func() = 0;

    unsigned long _max_time_slice_us;
    unsigned long _work_start_time;
};