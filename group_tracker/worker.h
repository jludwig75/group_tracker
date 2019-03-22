#pragma once

#include <stdint.h>

class Worker
{
public:
    virtual ~Worker();

    virtual void do_work() = 0;
};