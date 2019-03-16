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

#if 1   //ENABLE_DEBUGLOGGING
#define DBG_LOG_ERROR(msg, ...) debug_log.error(msg, ##__VA_ARGS__)
#define DBG_LOG_INFO(msg, ...)  debug_log.info(msg, ##__VA_ARGS__)
#define DBG_LOG_DEBUG(msg, ...) debug_log.debug(msg, ##__VA_ARGS__)
#else   // ENABLE_DEBUGLOGGING
#define DBG_LOG_ERROR(msg, ...)
#define DBG_LOG_INFO(msg, ...)
#define DBG_LOG_DEBUG(msg, ...)
#endif  // ENABLE_DEBUGLOGGING
