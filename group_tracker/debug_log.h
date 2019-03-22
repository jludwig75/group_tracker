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

#define DBG_LOG_LEVEL_ERROR 1
#define DBG_LOG_LEVEL_INFO  2
#define DBG_LOG_LEVEL_DEBUG 3


#ifdef  DBG_LOG_LEVEL

  #if DBG_LOG_LEVEL > 0
  
    #define DBG_LOG_ERROR(msg, ...) debug_log.error(msg, ##__VA_ARGS__)
  
    #if DBG_LOG_LEVEL > 1
    
      #define DBG_LOG_INFO(msg, ...)  debug_log.info(msg, ##__VA_ARGS__)
    
      #if DBG_LOG_LEVEL > 2
      
        #define DBG_LOG_DEBUG(msg, ...) debug_log.debug(msg, ##__VA_ARGS__)
      
      #else  // DBG_LOG_LEVEL > 2
      
        #define DBG_LOG_DEBUG(msg, ...)
      
      #endif // DBG_LOG_LEVEL > 2
  
    #else  // DBG_LOG_LEVEL > 1
    
      #define DBG_LOG_INFO(msg, ...)
      #define DBG_LOG_DEBUG(msg, ...)
    
    #endif // DBG_LOG_LEVEL > 1
  
  #else  // DBG_LOG_LEVEL > 0
  
    #define DBG_LOG_ERROR(msg, ...)
    #define DBG_LOG_INFO(msg, ...)
    #define DBG_LOG_DEBUG(msg, ...)
  
  #endif // DBG_LOG_LEVEL > 0

#else   // DBG_LOG_LEVEL

  #define DBG_LOG_ERROR(msg, ...)
  #define DBG_LOG_INFO(msg, ...)
  #define DBG_LOG_DEBUG(msg, ...)

#endif  // DBG_LOG_LEVEL
