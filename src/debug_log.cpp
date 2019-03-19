#include "debug_log.h"


#include <Print.h>

#ifndef PRINTF_BUF
#define PRINTF_BUF  80
#endif  // PRINTF_BUF

DebugLog::DebugLog(Print & printer) :
    _printer(printer)
{

}

void DebugLog::print_string(const char *str)
{
    for(const char *p = str; *p; p++) // emulate cooked mode for newlines
    {
        if(*p == '\n')
        {
            _printer.write('\r');
        }
        _printer.write(*p);
    }
}

void DebugLog::info(const char *format, ...)
{
    char buf[PRINTF_BUF];

    va_list ap;
    va_start(ap, format);

    vsnprintf(buf, sizeof(buf), format, ap);
    print_string(buf);

    va_end(ap);
}

void DebugLog::debug(const char *format, ...)
{
    char buf[PRINTF_BUF];

    va_list ap;
    va_start(ap, format);

    vsnprintf(buf, sizeof(buf), format, ap);
    print_string(buf);

    va_end(ap);
}

void DebugLog::error(const char *format, ...)
{
    char buf[PRINTF_BUF];

    va_list ap;
    va_start(ap, format);

    vsnprintf(buf, sizeof(buf), format, ap);
    print_string(buf);

    va_end(ap);
}
