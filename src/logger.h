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

#define logger(level, ...) logger_log(level, __func__, __VA_ARGS__)

void logger_log(LogLevel level, const char* function_name, const char *message, ...);
void logger_set_level(LogLevel level);
LogLevel logger_get_level();
LogLevel logger_level_from_string(const char* level_string);

#endif
