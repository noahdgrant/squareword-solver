#ifndef LOGGER_H
#define LOGGER_H

typedef enum {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
} LogLevel;

LogLevel logger_level_from_string(const char* level_string);
LogLevel logger_get_level();
void logger_set_level(LogLevel level);
void logger(LogLevel level, const char* function_name, const char *message, ...);

#endif
