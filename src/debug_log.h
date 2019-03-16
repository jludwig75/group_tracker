#pragma once


class Print;


class DebugLog
{
public:
    DebugLog(Print & printer);
    void info(const char *format, ...);
    void debug(const char *format, ...);
    void error(const char *format, ...);
private:
    void print_string(const char *str);
    Print & _printer;
};

extern DebugLog debug_log;
