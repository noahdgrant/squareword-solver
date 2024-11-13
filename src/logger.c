#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "logger.h"

static LogLevel m_logger_level = INFO;

static const char* logger_level_to_string(LogLevel level) {
    const char* level_str;
    switch (level) {
        case TRACE:
            level_str = "TRACE";
            break;
        case DEBUG:
            level_str = "DEBUG";
            break;
        case INFO:
            level_str = "INFO";
            break;
        case WARNING:
            level_str = "WARNING";
            break;
        case ERROR:
            level_str = "ERROR";
            break;
        case FATAL:
            level_str = "FATAL";
            break;
        default:
            logger(FATAL, "Unknown logger level: %d", level);
            exit(1);
            break;
    }
    return level_str;
}

LogLevel logger_level_from_string(const char* level_str) {
    LogLevel level = FATAL;
    if (strcmp(level_str, "TRACE") == 0) {
        level = TRACE;
    } else if (strcmp(level_str, "DEBUG") == 0) {
        level = DEBUG;
    } else if (strcmp(level_str, "INFO") == 0) {
        level = INFO;
    } else if (strcmp(level_str, "WARNING") == 0) {
        level = WARNING;
    } else if (strcmp(level_str, "ERROR") == 0) {
        level = ERROR;
    } else if (strcmp(level_str, "FATAL") == 0) {
        level = FATAL;
    } else {
        logger(FATAL, "Unknown logger level: %s", level_str);
        exit(1);
    }
    return level;
}

LogLevel logger_get_level() {
    return m_logger_level;
}

void logger_set_level(LogLevel level) {
    m_logger_level = level;
    return;
}

void logger_log(LogLevel level, const char* function_name, const char *message, ...) {
    if (level < m_logger_level) {
        return;
    }

    setvbuf(stderr, NULL, _IOLBF, 0);

    time_t now;
    time(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    const char *level_str = logger_level_to_string(level);

    flockfile(stderr);

    fprintf(stderr, "%s | %s | %s | ", time_str, level_str, function_name);

    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);

    fprintf(stderr, "\n");

    fflush(stderr);
    funlockfile(stderr);

    return;
}
